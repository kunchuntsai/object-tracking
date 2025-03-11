#include "config.h"
#include "logger.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>


// Trim function
static inline std::string trim(const std::string &s) {
    auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c){return std::isspace(c);});
    auto wsback = std::find_if_not(s.rbegin(), s.rend(), [](int c){return std::isspace(c);}).base();
    return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
}

// New function to remove comments
static inline std::string removeComment(const std::string &s) {
    size_t pos = s.find(';');
    if (pos != std::string::npos) {
        return s.substr(0, pos);
    }
    return s;
}

bool Config::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open config file: %s", filename.c_str());
        return false;
    }

    std::string line;
    std::string section;

    bool sourceSpecified = false;
    bool videoPathSpecified = false;

    // Set default log level mask
    logLevelMask = LOG_LV_ERROR | LOG_LV_WARNING | LOG_LV_INFO;

    while (std::getline(file, line)) {
        std::istringstream is_line(line);
        std::string key;

        if (line[0] == '[' && line[line.size() - 1] == ']') {
            section = line.substr(1, line.size() - 2);
        } else if (std::getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value)) {
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                if (section == "Model") {
                    if (key == "path") {
                        if (value.substr(value.length() - 5) == ".onnx") {
                            modelPath = value;
                        } else {
                            throw std::runtime_error("Invalid model path: File must have .onnx extension");
                        }
                    }
                    else if (key == "confidence_threshold") confidenceThreshold = std::stof(value);
                } else if (section == "Input") {
                    if (key == "source") {
                        sourceSpecified = true;
                        std::string trimmedValue = trim(removeComment(value));
                        std::string lowerValue = trimmedValue;
                        std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(),
                                    [](unsigned char c){ return std::tolower(c); });
                        if (lowerValue == "camera") {
                            inputSource = InputSource::CAMERA;
                            LOG_INFO("Input source set to CAMERA");
                        } else if (lowerValue == "video") {
                            inputSource = InputSource::VIDEO;
                            LOG_INFO("Input source set to VIDEO");
                        } else {
                            LOG_WARNING("Invalid input source: '%s'. Using default (VIDEO).", trimmedValue.c_str());
                            inputSource = InputSource::VIDEO;
                        }
                    } else if (key == "video_path") {
                        videoPath = trim(removeComment(value));
                        videoPathSpecified = !videoPath.empty();
                        if (videoPathSpecified) {
                            LOG_INFO("Video path set to: %s", videoPath.c_str());
                        } else {
                            LOG_WARNING("Empty video path specified.");
                        }
                    }
                } else if (section == "Tracking") {
                    if (key == "iou_threshold") iouThreshold = std::stof(value);
                    else if (key == "max_frames_to_skip") maxFramesToSkip = std::stoi(value);
                } else if (section == "Logging") {
                    if (key == "debug") {
                        std::string trimmedValue = trim(removeComment(value));
                        std::transform(trimmedValue.begin(), trimmedValue.end(), trimmedValue.begin(),
                                    [](unsigned char c){ return std::tolower(c); });
                        if (trimmedValue == "true" || trimmedValue == "1" || trimmedValue == "yes") {
                            logLevelMask |= LOG_LV_DEBUG;
                            LOG_INFO("Debug logging enabled");
                        } else {
                            logLevelMask &= ~LOG_LV_DEBUG;
                            LOG_INFO("Debug logging disabled");
                        }
                    }
                }
            }
        }
    } //while()

    if (!sourceSpecified) {
        if (videoPathSpecified) {
            LOG_WARNING("Input source not specified. Using default (VIDEO) because video path is present.");
            inputSource = InputSource::VIDEO;
        } else {
            LOG_ERROR("Invalid configuration: Neither input source nor video path specified.");
            return false;
        }
    }

    if (inputSource == InputSource::VIDEO && !videoPathSpecified) {
        LOG_ERROR("Invalid configuration: Video source selected but no valid video path provided.");
        return false;
    }

    if (inputSource == InputSource::CAMERA && videoPathSpecified) {
        LOG_WARNING("Camera input selected but video path also specified. Video path will be ignored.");
        videoPath = "";
    }

    return true;
}