#pragma once

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

class GazeThread : public PeriodicThread {
public:
    bool isMovementDone;
    
    GazeThread(double period);
    ~GazeThread();
    
    bool configure();
    
    // getters for plotting
    double getErrorX() const { return errX; }
    double getErrorY() const { return errY; }
    double getEyeX() const { return eyeYawPosition; }
    double getEyeY() const { return eyeTiltPosition; }
    double getNeckPitch() const { return neckPitchPosition; }
    double getNeckYaw() const { return neckYawPosition; }
    
protected:
    void run() override;
    void threadRelease() override;
    
private:
    // yarp interfaces
    Network yarp;
    Property prop;
    PolyDriver robotHead;
    IPositionControl* ipc;
    IVelocityControl* ivc;
    IControlMode* icm;
    IEncoders* enc;
    BufferedPort<ImageOf<PixelRgb>> imagePort;
    
    // control state
    double eyeTiltPosition, eyeYawPosition;
    double eyeEncoderTiltPosition, eyeEncoderYawPosition;
    double neckPitchPosition, neckYawPosition;
    int errX, errY;
    int lastErrX, lastErrY;
    double integralX, integralY;  // integral terms for pid
    
    // control parameters
    static constexpr double Kp = 0.2;   // proportional gain
    static constexpr double Ki = 0.01;  // integral gain
    static constexpr double Kd = 0.05;  // derivative gain
    static constexpr double HEAD_GAIN = 1.2;  // head movement gain
    static constexpr double MAX_INTEGRAL = 10.0;  // anti-windup limit
    
    // image dimensions
    static constexpr double IMAGE_WIDTH = 320.0;
    static constexpr double IMAGE_HEIGHT = 240.0;
    static constexpr double CENTER_X = IMAGE_WIDTH / 2.0;
    static constexpr double CENTER_Y = IMAGE_HEIGHT / 2.0;
    
    // movement thresholds
    static constexpr double POSITION_THRESHOLD = 0.2;  // degrees
    static constexpr double ERROR_THRESHOLD = 1.0;     // pixels
};
