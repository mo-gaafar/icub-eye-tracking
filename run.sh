#!/bin/bash

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check if a process is running
is_running() {
    pgrep -f "$1" >/dev/null
}

# Function to check if YARP is responding
check_yarp() {
    for i in {1..10}; do
        if yarp detect | grep -q "FOUND"; then
            return 0
        fi
        sleep 1
    done
    return 1
}

# Check for required commands
if ! command_exists yarp; then
    echo "Error: YARP is not installed or not in PATH"
    exit 1
fi

if ! command_exists iCub_SIM; then
    echo "Error: iCub_SIM is not installed or not in PATH"
    exit 1
fi

if ! command_exists yarpview; then
    echo "Error: yarpview is not installed or not in PATH"
    exit 1
fi

# Detect the terminal emulator
if command_exists gnome-terminal; then
    TERM_EMU="gnome-terminal"
elif command_exists xterm; then
    TERM_EMU="xterm"
else
    echo "Error: No supported terminal emulator found (gnome-terminal or xterm)"
    exit 1
fi

# Function to open a new terminal window with a command
open_terminal() {
    local title="$1"
    local command="$2"
    
    if [ "$TERM_EMU" = "gnome-terminal" ]; then
        gnome-terminal --title="$title" -- bash -c "$command; exec bash"
    else
        xterm -T "$title" -e "bash -c '$command; exec bash'" &
    fi
}

# Get the absolute path to the script directory, handling spaces correctly
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Make shutdown script executable
chmod +x "$SCRIPT_DIR/shutdown.sh"

# Start YARP server if not running
if ! is_running "yarpserver"; then
    echo "Starting YARP server..."
    open_terminal "YARP Server" "yarpserver --write"
    
    # Wait for YARP to be ready
    echo "Waiting for YARP server to be ready..."
    if ! check_yarp; then
        echo "Error: YARP server failed to start properly"
        exit 1
    fi
fi

# Set software rendering for the simulator
export LIBGL_ALWAYS_SOFTWARE=1
export MESA_GL_VERSION_OVERRIDE=3.3

# Start iCub simulator in new terminal
echo "Starting iCub simulator..."
open_terminal "iCub Simulator" "iCub_SIM"

# Wait for simulator to be ready by checking for the world port
echo "Waiting for simulator to be ready..."
for i in {1..20}; do
    if yarp exists /icubSim/world; then
        break
    fi
    sleep 1
    if [ $i -eq 20 ]; then
        echo "Error: Simulator failed to start properly"
        "$SCRIPT_DIR/shutdown.sh"
        exit 1
    fi
done

# Additional delay for simulator initialization
sleep 5

# Start yarpview for camera feed
echo "Starting camera view..."
open_terminal "Camera View" "yarpview --name /viewer"
sleep 2

# Connect camera to viewer
echo "Connecting camera feed..."
yarp connect /icubSim/cam/left /viewer

# Run our gaze tracking program in new terminal
echo "Starting gaze tracking program..."
# Use quotes to handle spaces in paths
open_terminal "Gaze Tracking" "cd \"$SCRIPT_DIR/build/bin\" && ./gaze_control"

echo "All components started!"
echo "To shut down all components, run: ./shutdown.sh"
echo "Monitoring processes... (This window will stay open)"

# Keep monitoring the processes
while true; do
    if ! is_running "yarpserver" || ! is_running "iCub_SIM" || ! is_running "gaze_control" || ! is_running "yarpview"; then
        echo "One or more components have stopped unexpectedly!"
        "$SCRIPT_DIR/shutdown.sh"
        exit 1
    fi
    sleep 5
done
# If Ctrl+C is pressed, run ./shutdown.sh
echo "Shutting down..."
trap "$SCRIPT_DIR/shutdown.sh" SIGINT
"$SCRIPT_DIR/shutdown.sh"

