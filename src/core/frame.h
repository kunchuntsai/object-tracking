#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <onnxruntime_cxx_api.h>
#include <optional>

struct Frame {
    cv::Mat original;
    cv::Mat processed;
    std::optional<Ort::Value> onnx_input;
    std::vector<cv::Rect> detections;

    Frame() = default;
    Frame(const Frame&) = delete;
    Frame& operator=(const Frame&) = delete;
    Frame(Frame&&) = default;
    Frame& operator=(Frame&&) = default;
};