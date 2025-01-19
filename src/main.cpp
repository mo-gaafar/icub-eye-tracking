#include <yarp/os/all.h>
#include <iostream>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
#include <string>
#include <ctime>
#include <cmath>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace std;

class MyThread : public PeriodicThread {
public:
    MyThread(double period) : PeriodicThread(period) {
        bool ok = imagePort.open("/img");
        if (!ok) {
            cerr << "Failed to open the port" << endl;
            return;
        }
        yarp.connect("/icubSim/cam/left", "/img");

        // Creating the connection, associating names of the port
        prop.put("device", "remote_controlboard");
        prop.put("local", "/thread");
        prop.put("remote", "/icubSim/head");

        pd.open(prop); // starting the polydriver
        // Opening the controllers we want to use
        pd.view(ipc);
        pd.view(enc);
        encposition = 0.0;
    }

private:
    Network yarp;
    Property prop;
    PolyDriver pd;
    IPositionControl *ipc;
    IEncoders *enc;
    BufferedPort<ImageOf<PixelRgb>> imagePort;
    double pos = 0.0;
    double encposition;

protected:
    void run() override {
        ImageOf<PixelRgb> *image = imagePort.read();
        if (!image) return;

        int pixelmean = 0;
        int howmanypix = 0;
        
        for (int x = 0; x < image->width(); x++) {
            for (int y = 0; y < image->height(); y++) {
                PixelRgb &pixel = image->pixel(x, y);
                if (pixel.r > 2 * pixel.g && pixel.r > 2 * pixel.b) {
                    pixelmean += x;
                    howmanypix++;
                }
            }
        }

        if (howmanypix > 0) {
            pixelmean = pixelmean / howmanypix;
            int err = pixelmean - (image->width() / 2);
            double deg = err / 4.0;
            
            bool ok = enc->getEncoder(4, &encposition);
            cout << "Encoder value: " << encposition << endl;

            if (!ok) {
                cerr << "Failed to read the encoder" << endl;
                return;
            }

            pos = encposition;
            pos += deg;
            ipc->positionMove(4, pos);
        }
    }

    void threadRelease() override {
        pd.close();
    }
};

int main(int argc, char *argv[]) {
    // Set up YARP
    Network yarp;
    
    // Create an RPC port
    RpcClient rpc;
    bool ok = rpc.open("/sphere");
    if (!ok) {
        cerr << "Failed to open the port" << endl;
        return 1;
    }

    yarp.connect("/sphere", "/icubSim/world");

    Bottle b, reply;

    // Clear the world
    b.addString("world");
    b.addString("del");
    b.addString("all");
    rpc.write(b, reply);

    // Create a red sphere
    b.clear();
    reply.clear();
    b.addString("world");
    b.addString("mk");
    b.addString("ssph");
    b.addFloat64(0.04);    // radius
    b.addFloat64(-0.3);    // x
    b.addFloat64(0.9);     // y
    b.addFloat64(0.8);     // z
    b.addInt32(1);         // red
    b.addInt32(0);         // green
    b.addInt32(0);         // blue

    rpc.write(b, reply);
    cout << "Sphere creation response: " << reply.toString() << endl;

    // Start the tracking thread
    double x = -0.3;
    double step;
    MyThread mt(0.5);
    mt.start();

    // Move the sphere back and forth
    for (int iter = 0; iter < 4; iter++) {
        step = (iter % 2 == 0) ? 0.005 : -0.005;
        
        for (int ii = 0; ii < 120; ii++) {
            b.clear();
            x += step;
            
            b.addString("world");
            b.addString("set");
            b.addString("ssph");
            b.addInt32(1);
            b.addFloat64(x);
            b.addFloat64(0.9);
            b.addFloat64(0.8);

            rpc.write(b, reply);
            Time::delay(0.1);
        }
    }

    return 0;
}
