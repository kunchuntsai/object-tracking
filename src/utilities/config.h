/**
 * @file config.h
 * @brief Configuration class for managing application settings
 */

#pragma once

#include <string>

/**
 * @class Config
 * @brief Manages configuration settings for the application
 */
class Config {
public:
    /**
     * @enum InputSource
     * @brief Specifies the source of input for the application
     */
    enum class InputSource {
        VIDEO,  /**< Input from a video file */
        CAMERA  /**< Input from a camera */
    };

    /**
     * @brief Loads configuration from a file
     * @param filename The path to the configuration file
     * @return true if loading was successful, false otherwise
     */
    static bool loadFromFile(const std::string& filename);

    /**
     * @brief Sets the input source
     * @param source The input source to set
     */
    static void setInputSource(InputSource source) {
        inputSource = source;
    }

    /**
     * @brief Gets the current input source
     * @return The current input source
     */
    static InputSource getInputSource() {
        return inputSource;
    }

    /**
     * @brief Sets the path to the video file
     * @param path The path to the video file
     */
    static void setVideoPath(const std::string& path) {
        videoPath = path;
    }

    /**
     * @brief Gets the path to the video file
     * @return The path to the video file
     */
    static std::string getVideoPath() {
        return videoPath;
    }

    /**
     * @brief Gets the path to the model file
     * @return The path to the model file
     */
    static std::string getModelPath() { return modelPath; }

    /**
     * @brief Gets the confidence threshold
     * @return The confidence threshold
     */
    static float getConfidenceThreshold() { return confidenceThreshold; }

    /**
     * @brief Gets the IoU threshold
     * @return The IoU threshold
     */
    static float getIoUThreshold() { return iouThreshold; }

    /**
     * @brief Gets the maximum number of frames to skip
     * @return The maximum number of frames to skip
     */
    static int getMaxFramesToSkip() { return maxFramesToSkip; }

    /**
     * @brief Gets the log level mask
     * @return The log level mask
     */
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