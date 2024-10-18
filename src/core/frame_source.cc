#include "frame_source.h"
#include "logger.h"
#include "config.h"

FrameSource& FrameSource::getInstance() {
    static FrameSource instance;
    return instance;
}

bool FrameSource::initialize() {
    if (Config::getInputSource() == Config::InputSource::CAMERA) {
        cap.open(0);  // Open default camera
    } else {
        cap.open(Config::getVideoPath());
    }

    if (!cap.isOpened()) {
        LOG_ERROR("Failed to open %s",
            (Config::getInputSource() == Config::InputSource::CAMERA ? "camera" : "video file"));
        return false;
    }

    LOG_INFO("Frame source initialized successfully");
    return true;
}

bool FrameSource::getNextFrame(Frame& frame) {
    if (!cap.isOpened()) {
        return false;
    }
    return cap.read(frame.original);
}