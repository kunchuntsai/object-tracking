/**
 * @file display.h
 * @brief Header file for the Display class, responsible for visualizing frames.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <opencv2/opencv.hpp>
#include "frame.h"
#include <chrono>
#include <string>

/**
 * @class Display
 * @brief Manages the display of frames and associated information.
 */
class Display {
public:
    /**
     * @brief Constructor for the Display class.
     */
    Display();

    /**
     * @brief Destructor for the Display class.
     */
    ~Display();

    /**
     * @brief Display a frame with optional bounding boxes and FPS information.
     * @param frame The Frame object to be displayed.
     */
    void showFrame(const Frame& frame);

    /**
     * @brief Toggle the display of bounding boxes on and off.
     */
    void toggleBoundingBoxes();

    /**
     * @brief Close the display window.
     */
    void close();

    /**
     * @brief Set the current FPS value.
     * @param newFPS The new FPS value to be displayed.
     */
    void setFPS(double newFPS) { fps = newFPS; }

private:
    std::string windowName; ///< Name of the display window
    bool showBoundingBoxes; ///< Flag to control bounding box display
    double fps; ///< Current FPS to be displayed
};

#endif // DISPLAY_H