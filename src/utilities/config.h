#pragma once

#include <string>

class Config {
public:
    enum class InputSource {
        VIDEO,
        CAMERA
    };

    static bool loadFromFile(const std::string& filename);

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

    static std::string getModelPath() { return modelPath; }
    static float getConfidenceThreshold() { return confidenceThreshold; }
    static float getIoUThreshold() { return iouThreshold; }
    static int getMaxFramesToSkip() { return maxFramesToSkip; }
    static int getLogLevelMask() { return logLevelMask; }

private:
    static inline InputSource inputSource = InputSource::VIDEO;
    static inline std::string videoPath = "";
    static inline std::string modelPath = "";
    static inline float confidenceThreshold = 0.5f;
    static inline float iouThreshold = 0.5f;
    static inline int maxFramesToSkip = 10;
    static inline int logLevelMask = 0;
};