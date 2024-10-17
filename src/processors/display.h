#ifndef DISPLAY_H
#define DISPLAY_H

#include <opencv2/opencv.hpp>
#include "frame.h"
#include <chrono>

class Display {
public:
    Display();
    ~Display();

    void showFrame(const Frame& frame);
    void toggleBoundingBoxes();
    void close();

private:
    std::string windowName;
    bool showBoundingBoxes;

    // FPS calculation
    int frameCount;
    double fps;
    std::chrono::steady_clock::time_point lastFPSUpdateTime;

    void updateFPS();
};

#endif // DISPLAY_H