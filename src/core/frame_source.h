/**
 * @file frame_source.h
 * @brief Defines the FrameSource class for video frame acquisition
 */

#pragma once

#include <opencv2/opencv.hpp>
#include "frame.h"

/**
 * @class FrameSource
 * @brief Singleton class for acquiring video frames
 */
class FrameSource {
public:
    /**
     * @brief Get the singleton instance of FrameSource
     * @return Reference to the FrameSource instance
     */
    static FrameSource& getInstance();

    /**
     * @brief Initialize the frame source
     * @return true if initialization was successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get the next frame from the source
     * @param frame Reference to a Frame object to store the acquired frame
     * @return true if a frame was successfully acquired, false otherwise
     */
    bool getNextFrame(Frame& frame);

private:
    FrameSource() = default;
    ~FrameSource() = default;
    FrameSource(const FrameSource&) = delete;
    FrameSource& operator=(const FrameSource&) = delete;

    cv::VideoCapture cap; /**< OpenCV VideoCapture object for frame acquisition */
};