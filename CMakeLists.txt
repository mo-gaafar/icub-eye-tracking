cmake_minimum_required(VERSION 3.12)
project(icub_eye_tracking)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find YARP
find_package(YARP COMPONENTS os sig dev REQUIRED)

# Add the src directory
add_subdirectory(src)