#include <opencv2/opencv.hpp>
#include "display.h"
#include <chrono>
#include <iomanip>
#include <sstream>

Display::Display()
    : windowName("Object Tracking"),
      showBoundingBoxes(true),
      fps(0.0) {
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

    // Draw bounding boxes and track IDs on the display frame if enabled
    if (showBoundingBoxes) {
        for (size_t i = 0; i < frame.detections.size(); ++i) {
            const auto& bbox = frame.detections[i];
            int trackID = frame.trackIDs[i];

            // Scale the bounding box coordinates
            cv::Rect scaledRect(
                cv::Point(bbox.x * scaleX, bbox.y * scaleY),
                cv::Point((bbox.x + bbox.width) * scaleX, (bbox.y + bbox.height) * scaleY)
            );

            // Draw the bounding box in green
            cv::rectangle(displayFrame, scaledRect, cv::Scalar(0, 255, 0), 2, 8, 0);

            // Draw the track ID
            if (trackID != -1) {
                std::string idText = std::to_string(trackID);
                double fontScale = 1.5;
                int thickness = 2;
                cv::Point textPos(scaledRect.x, scaledRect.y - 10);

                cv::putText(displayFrame, idText, textPos, cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(0, 0, 0), thickness + 2);
                cv::putText(displayFrame, idText, textPos, cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(0, 255, 0), thickness);
            }
        }
    }

    // Format FPS string with two decimal places
    std::ostringstream fpsStream;
    fpsStream << "FPS: " << std::fixed << std::setprecision(2) << fps;

    cv::putText(displayFrame, fpsStream.str(), cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);

    cv::imshow(windowName, displayFrame);
}

void Display::toggleBoundingBoxes() {
    showBoundingBoxes = !showBoundingBoxes;
}

void Display::close() {
    cv::destroyWindow(windowName);
}