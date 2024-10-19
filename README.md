# Object Tracking

![Version](https://img.shields.io/badge/version-0.1.0-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

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

## Getting Started

### Prerequisites

Ensure you have the following dependencies installed:

* OpenCV (version 4.0 or later recommended)
* ONNX Runtime (version 1.8 or later recommended)
* Nix (optional, for Nix-based builds)

### Installation

1. Clone the repository:
   ```
   git clone https://github.com/yourusername/object-tracking.git
   cd object-tracking
   ```

2. Build the project:
   - For standard build:
     ```
     ./scripts/build.sh 0.1.0  # or your desired version
     ```
   - For Nix-based build:
     ```
     direnv allow
     nix flake update
     nix build
     ```

## ONNX Model

This project uses a YOLOv7 model. Please refer to [Simple-ONNX-runtime-c-example](https://github.com/JINSCOTT/Simple-ONNX-runtime-c-example).

To use a custom ONNX model:
1. Place your `.onnx` file in a directory
2. Update the model path in `config/config.ini`

## Usage

Run the program:
```
./build/object-tracking config.ini  # for standard build
./result/bin/object-tracking config.ini  # for Nix-based build
```

Perform the tests for logger and onnx loading
```
./build/run_tests <path-to-model>  # for standard build
./result/bin/run_tests <path-to-model>  # for Nix-based build
```

### Runtime Controls

- `Q` or `q`: Terminate the program
- `C` or `c`: Toggle continuous mode
- `Space`: Advance to the next frame
- `B` or `b`: Show/hide bounding boxes

## Documentation

To generate documentation:

1. Ensure Doxygen is installed
2. Run `doxygen doc/Doxyfile` from the project root
3. Open `doc/html/index.html` in a web browser

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.