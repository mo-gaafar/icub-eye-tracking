#include "GazeThread.h"
#include <yarp/os/LogStream.h>
#include <cmath>

GazeThread::GazeThread(double period) : 
    PeriodicThread(period),
    isMovementDone(false),
    eyeTiltPosition(0.0),
    eyeYawPosition(0.0),
    eyeEncoderTiltPosition(0.0),
    eyeEncoderYawPosition(0.0),
    neckPitchPosition(0.0),
    neckYawPosition(0.0),
    errX(0),
    errY(0),
    lastErrX(0),
    lastErrY(0),
    integralX(0.0),
    integralY(0.0) {
}

GazeThread::~GazeThread() {
    threadRelease();
}

bool GazeThread::configure() {
    // open image input port
    if (!imagePort.open("/gazeControl/img:i")) {
        yError() << "failed to open image port";
        return false;
    }
    yarp.connect("/icubSim/cam/left", "/gazeControl/img:i");
    
    // configure robot head
    prop.put("device", "remote_controlboard");
    prop.put("local", "/gazeControl");
    prop.put("remote", "/icubSim/head");
    
    if (!robotHead.open(prop)) {
        yError() << "failed to connect to robot head";
        return false;
    }
    
    // get interfaces
    bool ok = robotHead.view(ipc) &&
              robotHead.view(enc) &&
              robotHead.view(ivc) &&
              robotHead.view(icm);
              
    if (!ok) {
        yError() << "failed to get interfaces";
        robotHead.close();
        return false;
    }
    
    // initialize control modes
    icm->setControlMode(0, VOCAB_CM_POSITION);  // neck pitch
    icm->setControlMode(2, VOCAB_CM_POSITION);  // neck yaw
    icm->setControlMode(3, VOCAB_CM_POSITION);  // eyes tilt
    icm->setControlMode(4, VOCAB_CM_POSITION);  // eyes pan
    
    // move to initial position
    ipc->positionMove(0, 0.0);
    ipc->positionMove(2, 0.0);
    ipc->positionMove(3, 0.0);
    ipc->positionMove(4, 0.0);
    
    yarp::os::Time::delay(2.0);  // wait for initial positioning
    
    // switch neck to velocity control
    icm->setControlMode(0, VOCAB_CM_VELOCITY);
    icm->setControlMode(2, VOCAB_CM_VELOCITY);
    ivc->velocityMove(0, 0.0);
    ivc->velocityMove(2, 0.0);
    
    return true;
}

void GazeThread::run() {
    // get image
    ImageOf<PixelRgb>* image = imagePort.read();
    if (!image) return;
    
    // find red pixels
    int pixelMeanX = 0, pixelMeanY = 0;
    int numRedPixels = 0;
    
    for (size_t x = 0; x < image->width(); x++) {
        for (size_t y = 0; y < image->height(); y++) {
            const PixelRgb& pixel = image->pixel(x, y);
            if (pixel.r > 2*pixel.g && pixel.r > 2*pixel.b) {
                pixelMeanX += x;
                pixelMeanY += y;
                numRedPixels++;
            }
        }
    }
    
    if (numRedPixels < 50) return;  // not enough red pixels found
    
    // calculate centroid
    pixelMeanX /= numRedPixels;
    pixelMeanY /= numRedPixels;
    
    // calculate error from center
    errX = pixelMeanX - (image->width() / 2);
    errY = pixelMeanY - (image->height() / 2);
    
    // update integral terms with anti-windup
    integralX += errX;
    integralY += errY;
    
    // apply anti-windup by clamping integral terms
    integralX = std::max(-MAX_INTEGRAL, std::min(MAX_INTEGRAL, integralX));
    integralY = std::max(-MAX_INTEGRAL, std::min(MAX_INTEGRAL, integralY));
    
    // pid control
    double degX = errX * Kp +                    // proportional
                 integralX * Ki +                // integral
                 (lastErrX - errX) * Kd;         // derivative
                 
    double degY = errY * Kp +                    // proportional
                 integralY * Ki +                // integral
                 (lastErrY - errY) * Kd;         // derivative
    
    // get current eye positions
    enc->getEncoder(4, &eyeEncoderYawPosition);   // eye yaw
    enc->getEncoder(3, &eyeEncoderTiltPosition);  // eye tilt
    enc->getEncoder(0, &neckPitchPosition);       // neck pitch
    enc->getEncoder(2, &neckYawPosition);         // neck yaw
    
    // update eye positions
    eyeYawPosition = eyeEncoderYawPosition + degX;
    eyeTiltPosition = eyeEncoderTiltPosition - degY;  // negative because image y is inverted
    
    // move eyes
    ipc->positionMove(4, eyeYawPosition);
    ipc->positionMove(3, eyeTiltPosition);
    
    // head compensation when eyes are not centered
    if (std::abs(eyeEncoderYawPosition) > 0.1 || std::abs(eyeEncoderTiltPosition) > 0.1) {
        double neckPitchVel = eyeTiltPosition * HEAD_GAIN;
        double neckYawVel = -eyeYawPosition * HEAD_GAIN;
        
        ivc->velocityMove(0, neckPitchVel);    // neck pitch
        ivc->velocityMove(2, neckYawVel);      // neck yaw
    } else {
        ivc->velocityMove(0, 0.0);
        ivc->velocityMove(2, 0.0);
        
        // reset integral terms when centered to prevent drift
        integralX = 0.0;
        integralY = 0.0;
    }
    
    // store errors for derivative control
    lastErrX = errX;
    lastErrY = errY;
    
    // check if movement is complete
    isMovementDone = (std::abs(errX) < ERROR_THRESHOLD &&
                     std::abs(errY) < ERROR_THRESHOLD &&
                     std::abs(eyeEncoderTiltPosition) < POSITION_THRESHOLD &&
                     std::abs(eyeEncoderYawPosition) < POSITION_THRESHOLD);
}

void GazeThread::threadRelease() {
    // stop all movements
    if (ivc) {
        ivc->velocityMove(0, 0.0);
        ivc->velocityMove(2, 0.0);
    }
    
    // close devices and ports
    robotHead.close();
    imagePort.close();
}
