#include "GazeThread.h"
#include "PlotWindow.h"
#include <yarp/os/LogStream.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <QApplication>

// Sphere parameters
const double SPHERE_RADIUS = 0.04;
const double SPHERE_Z = 0.8;

// Movement boundaries
const double MIN_X = -0.3;
const double MAX_X = 0.3;
const double MIN_Y = 0.6;
const double MAX_Y = 1.1;

class GazeControlApp {
public:
    GazeControlApp() : plotWindow(nullptr) {}
    
    bool configure() {
        if (!yarp.checkNetwork()) {
            yError() << "YARP network not available";
            return false;
        }

        // Open RPC port for simulator control
        if (!worldPort.open("/gazeControl/world:o")) {
            yError() << "Failed to open world port";
            return false;
        }
        yarp.connect("/gazeControl/world:o", "/icubSim/world");

        // Create initial sphere
        if (!createSphere()) {
            yError() << "Failed to create sphere";
            return false;
        }
        yInfo() << "Created sphere successfully";

        // Start gaze control thread
        gazeControl.reset(new GazeThread(0.02));  // 50Hz control loop
        if (!gazeControl->configure()) {
            yError() << "Failed to configure gaze control";
            return false;
        }
        gazeControl->start();
        yInfo() << "Started gaze control thread";
        
        // Create plot window
        plotWindow = new PlotWindow();
        plotWindow->show();
        
        return true;
    }
    
    void run() {
        // Initialize last position
        double lastX = 0.0;
        double lastY = 0.9;
        
        // Main loop - move sphere randomly
        for (int iter = 0; iter < 6 && !isStopping; iter++) {
            yInfo() << "Iteration" << iter + 1 << "of 6";
            
            // Generate new position
            double newX, newY;
            generateNewPosition(lastX, lastY, newX, newY);
            
            yInfo() << "Moving sphere to (x,y) = (" << newX << "," << newY << ")";
            
            if (!moveSphere(newX, newY)) {
                yError() << "Failed to move sphere";
                continue;
            }
            
            gazeControl->isMovementDone = false;
            
            // Wait for gaze to stabilize
            while (!gazeControl->isMovementDone && !isStopping) {
                // Update plot with current errors and positions
                if (plotWindow) {
                    plotWindow->addDataPoint(
                        gazeControl->getErrorX(),
                        gazeControl->getErrorY(),
                        gazeControl->getEyeX(),
                        gazeControl->getEyeY(),
                        gazeControl->getNeckPitch(),
                        gazeControl->getNeckYaw()
                    );
                }
                yarp::os::Time::delay(0.1);
            }
            
            yInfo() << "Gaze stabilized at position" << iter + 1;
            yarp::os::Time::delay(2.0);  // Pause at each position
            
            // Update last position
            lastX = newX;
            lastY = newY;
        }
    }
    
    void stop() {
        isStopping = true;
        if (gazeControl) {
            gazeControl->stop();
        }
        worldPort.close();
        delete plotWindow;
    }

private:
    yarp::os::Network yarp;
    yarp::os::RpcClient worldPort;
    std::unique_ptr<GazeThread> gazeControl;
    PlotWindow* plotWindow;
    bool isStopping{false};
    
    bool createSphere() {
        yarp::os::Bottle cmd, reply;
        
        // Clear any existing objects
        cmd.addString("world");
        cmd.addString("del");
        cmd.addString("all");
        worldPort.write(cmd, reply);
        
        // Create new sphere
        cmd.clear();
        cmd.addString("world");
        cmd.addString("mk");
        cmd.addString("ssph");
        cmd.addFloat64(SPHERE_RADIUS);
        cmd.addFloat64(0.0);      // Initial X
        cmd.addFloat64(0.9);      // Initial Y
        cmd.addFloat64(SPHERE_Z);
        cmd.addFloat64(1.0);  // Red
        cmd.addFloat64(0.0);  // Green
        cmd.addFloat64(0.0);  // Blue
        
        return worldPort.write(cmd, reply);
    }
    
    bool moveSphere(double x, double y) {
        yarp::os::Bottle cmd, reply;
        cmd.addString("world");
        cmd.addString("set");
        cmd.addString("ssph");
        cmd.addInt32(1);  // Sphere ID
        cmd.addFloat64(x);
        cmd.addFloat64(y);
        cmd.addFloat64(SPHERE_Z);
        
        return worldPort.write(cmd, reply);
    }
    
    void generateNewPosition(double lastX, double lastY, double& newX, double& newY) {
        srand(time(0));
        
        do {
            float randomRangeX = -0.1 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(0.2)));
            float randomRangeY = -0.1 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(0.2)));
            
            newX = (randomRangeX < 0) ? lastX + randomRangeX - 0.2 : lastX + randomRangeX + 0.2;
            newY = (randomRangeY < 0) ? lastY + randomRangeY - 0.2 : lastY + randomRangeY + 0.2;
            
            newX = std::max(MIN_X, std::min(MAX_X, newX));
            newY = std::max(MIN_Y, std::min(MAX_Y, newY));
            
        } while (std::abs(newX - lastX) < 0.2 || std::abs(newY - lastY) < 0.2);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    GazeControlApp gazeApp;
    if (!gazeApp.configure()) {
        return 1;
    }
    
    // Run the gaze control in a separate thread
    std::thread gazeThread([&gazeApp]() {
        gazeApp.run();
    });
    
    // Run Qt event loop
    int result = app.exec();
    
    // Cleanup
    gazeApp.stop();
    if (gazeThread.joinable()) {
        gazeThread.join();
    }
    
    return result;
}
