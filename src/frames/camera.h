#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <stdexcept>

class Camera {
private:
    cv::VideoCapture cap;
    int width;
    int height;
    double fps;

public:
    Camera(int camera_index = 0, int width = 640, int height = 480, double fps = 30.0) 
        : width(width), height(height), fps(fps) {
        cap.open(camera_index);
        if (!cap.isOpened()) {
            throw std::runtime_error("Failed to open camera");
        }

        cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
        cap.set(cv::CAP_PROP_FPS, fps);
    }

    ~Camera() {
        if (cap.isOpened()) {
            cap.release();
        }
    }

    cv::Mat getFrame() {
        cv::Mat frame;
        if (!cap.read(frame)) {
            throw std::runtime_error("Failed to capture frame");
        }
        return frame;
    }

    bool isOpened() const {
        return cap.isOpened();
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    double getFPS() const { return fps; }
};