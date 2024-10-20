#!/bin/bash

# Check if a version argument is provided
if [ $# -eq 0 ]; then
    echo "No version provided. Using default version 0.1.0"
    VERSION="0.1.0"
else
    VERSION=$1
fi

# Get the directory of the script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# Navigate to the project root directory
cd "$SCRIPT_DIR/.."

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

# Use nix develop to run CMake and make
nix develop ..#default --command bash -c "cmake .. -DPROJECT_VERSION=$VERSION && make"

echo "Build complete. Run with: ./scripts/run.sh config/config.ini"