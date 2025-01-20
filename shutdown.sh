#!/bin/bash

# Function to kill processes by name
kill_process() {
    if pgrep "$1" > /dev/null; then
        echo "Stopping $1..."
        pkill -f "$1"
    fi
}

# Function to kill processes by port
kill_yarp_port() {
    if yarp exists "$1"; then
        echo "Closing YARP port $1..."
        yarp terminate "$1"
    fi
}

echo "Shutting down iCub Eye Tracking System..."

# Close YARP ports
kill_yarp_port "/gazeControl/world:o"
kill_yarp_port "/gazeControl/command:o"
kill_yarp_port "/gazeControl/rpc:o"
kill_yarp_port "/gazeControl/img:i"
kill_yarp_port "/gazeControl/stateExt:i"

# Kill Qt window and main program
kill_process "gaze_control"

# Kill simulator
kill_process "iCub_SIM"

# Clean YARP network
echo "Cleaning YARP network..."
yarp clean

# Kill yarpserver
kill_process "yarpserver"

echo "Shutdown complete!"
