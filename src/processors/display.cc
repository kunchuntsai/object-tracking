#include <opencv2/opencv.hpp>
#include "display.h"

Display::Display() : windowName("Object Tracking") {
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
}

Display::~Display() {
    close();
}

void Display::showFrame(const Frame& frame) {
    cv::Mat displayFrame = frame.processed.clone();  // Use the processed image from the Frame struct

    // Draw bounding boxes on the display frame
    for (const auto& bbox : frame.detections) {
        cv::rectangle(displayFrame, bbox.topLeft, bbox.bottomRight, cv::Scalar(0, 255, 0), 2);
    }

    cv::imshow(windowName, displayFrame);
}

void Display::close() {
    cv::destroyWindow(windowName);
}