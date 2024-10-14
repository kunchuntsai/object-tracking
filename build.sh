#!/bin/bash

# Check if a version argument is provided
if [ $# -eq 0 ]; then
    echo "No version provided. Using default version 0.1.0"
    VERSION="0.1.0"
else
    VERSION=$1
fi

# Check if build directory exists
if [ -d "build" ]; then
    echo "Build directory already exists. Cleaning..."
    rm -rf build/*
else
    echo "Creating build directory..."
    mkdir build
fi

# Navigate to build directory
cd build

# Run CMake and make
cmake .. -DPROJECT_VERSION=$VERSION
make

echo "Build complete. Run with: ./object-tracking [--camera | <path_to_video>]"