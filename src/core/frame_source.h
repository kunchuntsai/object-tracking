#pragma once

#include <opencv2/opencv.hpp>
#include "frame.h"


class FrameSource {
public:
    static FrameSource& getInstance();

    bool initialize();
    bool getNextFrame(Frame& frame);

private:
    FrameSource() = default;
    ~FrameSource() = default;
    FrameSource(const FrameSource&) = delete;
    FrameSource& operator=(const FrameSource&) = delete;

    cv::VideoCapture cap;
};