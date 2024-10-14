#pragma once

#include <string>

class Config {
public:
    enum class InputSource {
        VIDEO,
        CAMERA
    };

    static void setInputSource(InputSource source) {
        inputSource = source;
    }

    static InputSource getInputSource() {
        return inputSource;
    }

    static void setVideoPath(const std::string& path) {
        videoPath = path;
    }

    static std::string getVideoPath() {
        return videoPath;
    }

private:
    static InputSource inputSource;
    static std::string videoPath;
};

// Initialize static members
Config::InputSource Config::inputSource = Config::InputSource::VIDEO;
std::string Config::videoPath = "";