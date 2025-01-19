# Installation Guide for Ubuntu 22.04 (Jammy)

## Prerequisites

Install the required system packages:

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    cmake-curses-gui \
    git \
    pkg-config \
    libace-dev \
    libgsl-dev \
    libeigen3-dev \
    libopencv-dev \
    libode-dev \
    liblua5.1-dev \
    lua5.1 \
    qtbase5-dev \
    qtdeclarative5-dev \
    libqt5opengl5-dev \
    freeglut3-dev \
    libsdl1.2-dev \
    libxml2-dev \
    coinor-libipopt-dev \
    libglew-dev \
    libxmu-dev \
    mesa-utils \
    x11-apps
```

## Install YCM (YARP CMAKE MODULES)

```bash
cd /tmp
git clone --depth 1 --branch v0.14.2 https://github.com/robotology/ycm.git
cd ycm
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
sudo make install
```

## Install YARP

```bash
cd /tmp
git clone --depth 1 --branch v3.7.0 https://github.com/robotology/yarp.git
cd yarp
mkdir build && cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_yarpcar_mjpeg=ON \
    -DENABLE_yarpcar_depthimage=ON \
    -DENABLE_yarpcar_portmonitor=ON
make -j$(nproc)
sudo make install
sudo ldconfig
```

## Install iCub and Simulator

```bash
cd /tmp
git clone --depth 1 https://github.com/robotology/icub-main.git  -b v1.25.0
cd icub-main
mkdir build && cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_icubmod_cartesiancontrollerclient=ON \
    -DENABLE_icubmod_cartesiancontrollerserver=ON \
    -DENABLE_icubmod_gazecontrollerclient=ON \
    -DICUB_USE_IPOPT=ON \
    -DENABLE_icubmod_simcartesiancontrol=ON \
    -DENABLE_icubmod_simulator=ON \
    -DICUB_USE_SDL=ON \
    -DENABLE_icubmod_icubsimulation=ON \
    -DICUB_INSTALL_ALL_ROBOTS=ON \
    -DENABLE_icubmod_SimulatorWrapper=ON \
    -DENABLE_icubmod_SimulatorDevice=ON
make -j$(nproc)
sudo make install
sudo ldconfig
```

## Environment Setup

Add these lines to your `~/.bashrc`:

```bash
export YARP_ROOT=/usr/local
export YARP_DIR=/usr/local/lib/cmake/YARP
export YARP_DATA_DIRS=/usr/local/share/yarp:/usr/local/share/iCub
export ICUB_ROOT=/usr/local/share/iCub
export ICUB_DIR=/usr/local/share/iCub
export CMAKE_PREFIX_PATH=/usr/local/lib/cmake/YARP:/usr/local/share/cmake/YCM:${CMAKE_PREFIX_PATH}
export LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH}
```

Source the updated bashrc:
```bash
source ~/.bashrc
```

## Build the Project

```bash
cd /path/to/icub-eye-tracking
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Running the Application

1. Start the YARP server:
```bash
yarpserver
```

2. In a new terminal, start the iCub simulator:
```bash
iCub_SIM
```

3. In another terminal, run the eye tracking application:
```bash
cd /path/to/icub-eye-tracking/build
./icub_eye_tracking
```
