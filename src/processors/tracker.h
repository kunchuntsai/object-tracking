#pragma once

#include <opencv2/opencv.hpp>
#include "frame.h"
#include "thread_safe_queue.h"


class Tracker {
public:
    Tracker(ThreadSafeQueue<Frame>& input, ThreadSafeQueue<Frame>& output);
    void run();
    bool getProcessedFrame(Frame& frame);

private:
    ThreadSafeQueue<Frame>& inputQueue;
    ThreadSafeQueue<Frame>& outputQueue;

    BoundingBox createFixedBoundingBox(const cv::Mat& image);
};