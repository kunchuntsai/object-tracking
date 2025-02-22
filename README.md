# Object Tracking

![Version](https://img.shields.io/badge/version-0.1.0-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![macOS](https://img.shields.io/badge/verified%20on-macOS-brightgreen.svg)

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

### Prerequisites

Choose one of the following setup methods:

#### Docker Setup (Recommended)
* Docker Desktop
* XQuartz (for macOS)

#### Manual Setup
* OpenCV (version 4.7.0)
* ONNX Runtime (version 1.16.3)
* CMake
* C++ compiler

### Installation

1. Clone the repository:
```bash
git clone https://github.com/yourusername/object-tracking.git
cd object-tracking
```

2. Choose your build method:

#### Using Docker (Recommended)

1. Install XQuartz (macOS only):
```bash
brew install --cask xquartz
```

2. Configure XQuartz:
```bash
open -a XQuartz
```
* In XQuartz preferences (⌘,) → Security tab:
  - Enable "Allow connections from network clients"
  - **Important**: Restart XQuartz after changing settings

3. Setup X11 permissions:
```bash
xhost + localhost
```

4. Build and run:
```bash
# For a clean build (recommended for first time or after major changes)
docker compose build --no-cache

# Build and start the container
docker compose up --build

# Start the container
docker compose up
```

The container will mount:
- `./config`: Configuration files
- `./output`: Output directory
- `../_dataset`: Dataset directory
- X11 socket for display

#### Manual Build

1. Install dependencies (macOS):
```bash
brew install opencv
brew install onnxruntime
```

2. Build the project: (WIP)
```bash
# Build with default settings
./scripts/build.sh

# Or specify version and build type
./scripts/build.sh -v 0.1.1 -t Debug
```

WIP: The build script works identically in both Docker and manual environments, ensuring consistent builds across setups.

## Configuration

### Model Setup

This project uses a YOLOv7 model. Please refer to [Simple-ONNX-runtime-c-example](https://github.com/JINSCOTT/Simple-ONNX-runtime-c-example).

To use a custom ONNX model:
1. Place your `.onnx` file in a directory
2. Update the model path in `config/config.ini`

### Runtime Controls

* `Q` or `q`: Quit application
* `C` or `c`: Toggle continuous mode
* `Space`: Next frame
* `B` or `b`: Toggle bounding boxes

### Config File

Modify settings in `config/config.ini`:
* Input source (video/camera)
* Model parameters
* Display options

## Troubleshooting

### Common Issues

1. **X11 Display Issues (Docker)**
   * Error: "Could not connect to display" or "qt.qpa.xcb" errors
   * Fix: 
     - Ensure XQuartz is running
     - Run `xhost + localhost`
     - Check DISPLAY environment variable in docker-compose.yml
     - Restart XQuartz if needed

2. **Build Issues**
   * Error: CMake can't find ONNX Runtime
   * Fix: Verify ONNX Runtime installation path:
     - Docker: Check Dockerfile paths
     - Manual: Check brew installation (`brew info onnxruntime`)

3. **Performance Notes**
   * On M1/M2/M4 Macs: 
     - Application leverages ARM-specific optimizations
     - Custom build flags for M-series processors
     - ONNX Runtime configured for optimal performance

### Getting Help

For unresolved issues:
1. Check logs:
   * Docker: `docker compose logs`
   * Manual: Check build output
2. Open GitHub issue with:
   * System info (OS version, Docker/manual setup)
   * Full error logs
   * Steps to reproduce

## Development

### Testing

Run the tests:
```bash
./build/run_tests <path-to-model>
```

### Documentation

To generate documentation:
1. Ensure Doxygen is installed
2. Run:
```bash
doxygen doc/Doxyfile
```
3. Open `doc/html/index.html` in a browser

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.