#include <opencv2/opencv.hpp>
#include "display.h"

Display::Display() : windowName("Object Tracking"), showBoundingBoxes(true) {
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
}

Display::~Display() {
    close();
}

void Display::showFrame(const Frame& frame) {
    cv::Mat displayFrame;

    // Resize the processed frame back to original dimensions if needed
    if (frame.processed.size() != frame.original.size()) {
        cv::resize(frame.processed, displayFrame, frame.original.size());
    } else {
        displayFrame = frame.processed.clone();
    }

    // Calculate the scale factors
    float scaleX = static_cast<float>(displayFrame.cols) / frame.processed.cols;
    float scaleY = static_cast<float>(displayFrame.rows) / frame.processed.rows;

    // Draw bounding boxes on the display frame if enabled
    if (showBoundingBoxes) {
        for (const auto& bbox : frame.detections) {
            // Scale the bounding box coordinates
            cv::Point topLeft(bbox.topLeft.x * scaleX, bbox.topLeft.y * scaleY);
            cv::Point bottomRight(bbox.bottomRight.x * scaleX, bbox.bottomRight.y * scaleY);

            cv::rectangle(displayFrame, topLeft, bottomRight, cv::Scalar(0, 255, 0), 2);
        }
    }

    cv::imshow(windowName, displayFrame);
}

void Display::toggleBoundingBoxes() {
    showBoundingBoxes = !showBoundingBoxes;
}

void Display::close() {
    cv::destroyWindow(windowName);
}