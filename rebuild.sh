#!/bin/bash

# Stop on any error
set -e

echo "Removing build directory..."
rm -rf build

echo "Creating new build directory..."
mkdir build
cd build

echo "Running CMake..."
cmake ..

echo "Building..."
make -j$(nproc)

echo "Build complete! Executable is in build/bin/"
