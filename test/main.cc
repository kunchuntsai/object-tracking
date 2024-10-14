#include <iostream>
#include <string>
#include "main.h"

int main(int argc, char* argv[]) {
    // Set the initial log level (optional, INFO is default)
    Logger::getInstance().setLogLevel(Logger::LogLevel::DEBUG);
    
    if (argc < 2) {
        LOG_ERROR("Usage: " + std::string(argv[0]) + " [--camera | <path_to_video>]");
        return 1;
    }

    std::string input = argv[1];
    if (input == "--camera") {
        Config::setInputSource(Config::InputSource::CAMERA);
        LOG_DEBUG("Input source set to camera");
    } else {
        Config::setInputSource(Config::InputSource::VIDEO);
        Config::setVideoPath(input);
        LOG_DEBUG("Input source set to video: " + input);
    }

    LOG_DEBUG("Running all tests...");

    int result;
    if (Config::getInputSource() == Config::InputSource::CAMERA) {
        LOG_INFO("Running Camera test");
        result = runCamera();
    } else {
        LOG_INFO("Running Video test with video: " + Config::getVideoPath());
        result = runVideo(Config::getVideoPath());
    }

    if (result == 0) {
        LOG_INFO("Test completed successfully.");
    } else {
        LOG_ERROR("Test failed.");
    }

    return 0;
}