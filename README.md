# Object Tracking
## Description
This project provides C++ interface engine, image processing, performance optimization and simple user interface.

C++ Inference Engine
* Load and run inference using the trained model with ONNX format

Image Processing
* Read video frames from a file or camera stream.
* Preprocess the frames (resize, normalize) as required by the model.
* Run object detection inference on each frame.
* Apply tracking algorithms.
* Visualize the results (bounding boxes, object IDs) on the frames.

Performance Optimization
* Implement multi-threading to parallelize video decoding, inference, and visualization.
* Optimize the pipeline for real-time performance (aim for at least 15 FPS on a standard CPU).

Simple User Interface
* Select input video file or camera stream.
* Set detection confidence threshold.
* Toggle visualization options (e.g., show/hide bounding boxes, tracking IDs).

## Dependencies
Related libraries
* OpenCV
* ONNX runtime
* Nix: https://nixos.org/ (optional)

## How to Build and Run
Build the program
```
./build.sh 0.1.0  # or whatever version you want
```
If you have a nix setup environment, please build the project with `nix build`
```
direnv allow
nix flake show    # show flake.lock
nix flake update  # update flake.lock
nix develop
nix build
```

Run the program
```
Usage: ./object-tracking [--camera | <path_to_video>]

<path_to_executable>
./build/object-tracking      # normal build
./result/bin/object-tracking # nix configured

While running
- Keyboard: Q or q for terminate
- Keyboard: C or c for continuous
- Keyboard: Space for the next frame
```
