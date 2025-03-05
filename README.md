# Object Tracking

![Version](https://img.shields.io/badge/version-0.1.0-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![macOS](https://img.shields.io/badge/verified%20on-macOS-brightgreen.svg)

## Table of Contents

- [Description](#description)
  - [Key Features](#key-features)
  - [TODO](#todo)
- [Getting Started](#getting-started)
  - [Environment Setup - Docker (Recommended)](#environment-setup---docker-recommended)
  - [Environment Setup - Local Setup](#environment-setup--local-setup)
- [Project Configuration](#project-configuration)
  - [Model Setup](#model-setup)
  - [Runtime Controls](#runtime-controls)
  - [Config File](#config-file)
- [Development](#development)
- [Documentation](#documentation)
- [License](#license)

## Description

This project provides a C++ interface engine for object tracking, incorporating image processing, performance optimization, and a simple user interface.

### Key Features

1. **C++ Inference Engine**
   * Load and run inference using trained models in ONNX format

2. **Image Processing**
   * Read video frames from files or camera streams
   * Preprocess frames (resize, normalize) as required by the model
   * Run object detection inference on each frame
   * Apply tracking algorithms
   * Visualize results (bounding boxes, object IDs) on frames

3. **Performance Optimization**
   * Multi-threading to parallelize video decoding, inference, and visualization
   * Optimized pipeline for real-time performance (aim: 15+ FPS on standard CPU)

4. **Simple User Interface**
   * Select input video file or camera stream
   * Set detection confidence threshold
   * Toggle visualization options (e.g., show/hide bounding boxes, tracking IDs)

### TODO

* **Performance Improvement**
   * Install ONNX Runtime with CoreML support
   * Memory analysis to check memory footprint and avoid memory copy
   * SIMD vectorization, e.g. NEON instructions on an ARM machine

* **Testing and build module**
   * Include auto-run test modules in build process
   * Optimize CMake List to improve build time

## Getting Started

### Environment Setup - Docker (Recommended)

**Prerequisites:**
* Docker Desktop
* XQuartz (for macOS)

**Setup Steps:**

1. Install XQuartz (macOS only):
   ```bash
   brew install --cask xquartz
   ```

2. Configure XQuartz:
   ```bash
   open -a XQuartz
   ```
   In XQuartz preferences (⌘,) → Security tab:
   - Enable "Allow connections from network clients"
   - **Important**: Restart XQuartz after changing settings

3. Setup X11 permissions:
   ```bash
   xhost + localhost
   ```

4. Set up the Docker environment:
   ```bash
   # For a clean build (recommended for first time or after major changes)
   docker compose build --no-cache

   # Start the container in background mode
   docker compose up -d
   ```

   The container will mount:
   - `./config`: Configuration files
   - `./output`: Output directory
   - `../_dataset`: Dataset directory
   - X11 socket for display

5. Build the project inside Docker:
   ```bash
   # Make the build script executable
   chmod +x scripts/docker-build.sh

   # Build with default settings
   ./scripts/docker-build.sh

   # Or specify version and build type
   ./scripts/docker-build.sh -v 0.1.1 -t Debug
   ```

   The script will prompt you if you want to run the application after building.

### Environment Setup - Local Setup

**Prerequisites:**
* OpenCV (version 4.7.0)
* ONNX Runtime (version 1.16.3)
* CMake
* C++ compiler

**Setup Steps:**

1. Install dependencies (macOS):
   ```bash
   brew install opencv
   brew install onnxruntime
   ```

2. Build the project:
   ```bash
   # Make the build script executable
   chmod +x scripts/build.sh

   # Build with default settings
   ./scripts/build.sh

   # Or specify version and build type
   ./scripts/build.sh -v 0.1.1 -t Debug
   ```

3. Run the application:
   ```bash
   ./build/object-tracking ./config/config.ini
   ```

## Project Configuration

#### Model Setup

This project uses a YOLOv7 model. Please refer to [Simple-ONNX-runtime-c-example](https://github.com/JINSCOTT/Simple-ONNX-runtime-c-example).

To use a custom ONNX model:
1. Place your `.onnx` file in a directory
2. Update the model path in `config/config.ini`

#### Runtime Controls

* `Q` or `q`: Quit application
* `C` or `c`: Toggle continuous mode
* `Space`: Next frame
* `B` or `b`: Toggle bounding boxes

#### Config File

Modify settings in `config/config.ini`:
* Input source (video/camera)
* Model parameters
* Display options

## Development

Run the tests:
```bash
./build/run_tests <path-to-model>
```

## Documentation

To generate documentation:
1. Ensure Doxygen is installed
2. Run:
   ```bash
   doxygen doc/Doxyfile
   ```
3. Open `doc/html/index.html` in a browser

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.