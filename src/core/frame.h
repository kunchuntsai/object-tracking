/**
 * @file frame.h
 * @brief Frame data structure for object tracking pipeline
 */

#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <onnxruntime_cxx_api.h>
#include <optional>

/**
 * @brief Container for frame data throughout the object tracking pipeline
 *
 * This structure holds all data associated with a single video frame,
 * including the original image, processed version, ONNX model input tensor,
 * detected object bounding boxes, and associated tracking IDs.
 *
 * The Frame is move-only to avoid expensive copies of image data.
 */
struct Frame {
    /// Original unmodified frame from video source
    cv::Mat original;

    /// Processed/preprocessed frame ready for inference
    cv::Mat processed;

    /// Optional ONNX Runtime tensor input for model inference
    std::optional<Ort::Value> onnx_input;

    /// Bounding boxes of detected objects in the frame
    std::vector<cv::Rect> detections;

    /// Tracking IDs corresponding to each detection
    std::vector<int> trackIDs;

    /// Default constructor
    Frame() = default;

    /// Copy constructor (deleted - Frame is move-only)
    Frame(const Frame&) = delete;

    /// Copy assignment operator (deleted - Frame is move-only)
    Frame& operator=(const Frame&) = delete;

    /// Move constructor
    Frame(Frame&&) = default;

    /// Move assignment operator
    Frame& operator=(Frame&&) = default;
};