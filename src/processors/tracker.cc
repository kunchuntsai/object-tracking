#include "tracker.h"
#include "onnx_model.h"
#include "logger.h"

extern std::atomic<bool> shouldExit;

Tracker::Tracker(ThreadSafeQueue<Frame>& input, ThreadSafeQueue<Frame>& output)
    : inputQueue(input), outputQueue(output) {}

void Tracker::run() {
    while (!shouldExit) {
        Frame frame;
        if (inputQueue.pop(frame)) {
            if (frame.processed.empty()) { LOG_ERROR("Frame is empty"); continue; }

            // ONNX detector: std::vector<cv::Rect> detect(const cv::Mat& image);
            // BoundingBox detectedBox = ONNXModel::detect(frame.processed);
            BoundingBox detectedBox = createFixedBoundingBox(frame.processed); //Create a fixed bounding box
            frame.detections.clear();
            frame.detections.push_back(detectedBox);
            outputQueue.push(std::move(frame));
        }
    }
}

bool Tracker::getProcessedFrame(Frame& frame) {
    return outputQueue.pop(frame);
}

BoundingBox Tracker::createFixedBoundingBox(const cv::Mat& image) {
    int width = image.cols;
    int height = image.rows;

    // Calculate the top-left corner of the central 50x50 pixel rectangle
    int x = (width - 50) / 2;
    int y = (height - 50) / 2;

    // Create a BoundingBox for the central 50x50 pixel rectangle
    BoundingBox central_box;
    central_box.topLeft = cv::Point(x, y);
    central_box.bottomRight = cv::Point(x + 50, y + 50);

    return central_box;
}