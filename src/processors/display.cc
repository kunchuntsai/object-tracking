#include <opencv2/opencv.hpp>
#include "display.h"
#include <chrono>
#include <iomanip>
#include <sstream>

Display::Display()
    : windowName("Object Tracking"),
      showBoundingBoxes(true),
      frameCount(0),
      fps(0.0),
      lastFPSUpdateTime(std::chrono::steady_clock::now()) {
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
            cv::Rect scaledRect(
                cv::Point(bbox.x * scaleX, bbox.y * scaleY),
                cv::Point((bbox.x + bbox.width) * scaleX, (bbox.y + bbox.height) * scaleY)
            );

            // Draw the bounding box in green
            cv::rectangle(displayFrame, scaledRect, cv::Scalar(0, 255, 0), 2, 8, 0);
        }
    }

    // Update and display FPS
    updateFPS();

    // Format FPS string with two decimal places
    std::ostringstream fpsStream;
    fpsStream << "FPS: " << std::fixed << std::setprecision(2) << fps;

    cv::putText(displayFrame, fpsStream.str(), cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);

    cv::imshow(windowName, displayFrame);
}

void Display::updateFPS() {
    frameCount++;

    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFPSUpdateTime);

    if (elapsed.count() >= 1000) {  // Update every second
        fps = static_cast<double>(frameCount) * 1000.0 / elapsed.count();
        frameCount = 0;
        lastFPSUpdateTime = currentTime;
    }
}

void Display::toggleBoundingBoxes() {
    showBoundingBoxes = !showBoundingBoxes;
}

void Display::close() {
    cv::destroyWindow(windowName);
}