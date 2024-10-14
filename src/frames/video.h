#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

class Video {
private:
    cv::VideoCapture cap;
    int frameCount;
    double fps;
    int width;
    int height;

public:
    Video(const std::string& videoPath) {
        cap.open(videoPath);
        if (!cap.isOpened()) {
            throw std::runtime_error("Error opening video file");
        }
        
        frameCount = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
        fps = cap.get(cv::CAP_PROP_FPS);
        width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
        height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    }

    ~Video() {
        cap.release();
    }

    cv::Mat getFrame(int frameNumber) {
        cap.set(cv::CAP_PROP_POS_FRAMES, frameNumber);
        cv::Mat frame;
        cap.read(frame);
        return frame;
    }

    bool readNextFrame(cv::Mat& frame) {
        return cap.read(frame);
    }

    int getFrameCount() const { return frameCount; }
    double getFPS() const { return fps; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};