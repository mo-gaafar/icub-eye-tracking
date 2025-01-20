cmake_minimum_required(VERSION 3.10)
project(gaze_control)

# Find YARP
find_package(YARP REQUIRED)

# Find Qt
find_package(Qt5 COMPONENTS Widgets PrintSupport REQUIRED)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/qcustomplot
)

# Source files
set(SOURCES
    src/main.cpp
    src/GazeThread.cpp
    src/PlotWindow.cpp
    external/qcustomplot/qcustomplot.cpp
)

# Header files
set(HEADERS
    include/GazeThread.h
    include/PlotWindow.h
    external/qcustomplot/qcustomplot.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${YARP_LIBRARIES}
    Qt5::Widgets
    Qt5::PrintSupport
)

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Set C++ standard and warnings
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)