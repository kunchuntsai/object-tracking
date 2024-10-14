#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

struct BoundingBox {
    cv::Point topLeft;
    cv::Point bottomRight;
};

struct Frame {
    cv::Mat original;
    cv::Mat processed;
    std::vector<BoundingBox> detections;
};