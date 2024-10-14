#pragma once

#include <opencv2/opencv.hpp>
#include <stdexcept>

class ImageProcessor {
public:
    // Resize the input frame to the specified dimensions
    static cv::Mat resize(const cv::Mat& frame, int targetWidth, int targetHeight) {
        cv::Mat resizedFrame;
        cv::resize(frame, resizedFrame, cv::Size(targetWidth, targetHeight), 0, 0, cv::INTER_LINEAR);
        return resizedFrame;
    }

    // Normalize the pixel values to the range [0, 1]
    static cv::Mat normalize(const cv::Mat& frame) {
        cv::Mat normalizedFrame;
        frame.convertTo(normalizedFrame, CV_32F, 1.0 / 255.0);
        return normalizedFrame;
    }

    // Combine resize and normalize operations
    static cv::Mat processFrame(const cv::Mat& frame, int targetWidth, int targetHeight) {
        cv::Mat resizedFrame = resize(frame, targetWidth, targetHeight);
        cv::Mat normalizedFrame = normalize(resizedFrame);
        return normalizedFrame;
    }
};