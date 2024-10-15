#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include "config.h"
#include "logger.h"
#include "thread_safe_queue.h"
#include "frame.h"
#include "frame_source.h"
#include "onnx_model.h"
#include "display.h"
#include "preprocessor.h"
#include "tracker.h"


std::atomic<bool> shouldExit(false);
std::atomic<bool> continuousMode(false);


bool initialization(int argc, char* argv[]) {
    // 1. load a ONNX model: create a function to load an ONNX from a path; create a pseudo function with correct inputs/outputs for now
    std::string modelPath = argv[1];
    if (modelPath.length() < 5 || modelPath.substr(modelPath.length() - 5) != ".onnx") {
        LOG_ERROR("Invalid model file. Expected .onnx file, got: " + modelPath);
        return false;
    } else {
        LOG_DEBUG("Model path: " + modelPath);
    }

    //if (!ONNXModel::loadModel(modelPath)) { TODO_ONNX
    if (false) {
        LOG_ERROR("Failed to load ONNX model");
        return false;
    }

    // 2. read config: init logger, check if the frame source is from video or camera feed, model parameters
    std::string input = argv[2];
    if (input == "--camera") {
        Config::setInputSource(Config::InputSource::CAMERA);
        LOG_DEBUG("Input source set to camera");
    } else {
        Config::setInputSource(Config::InputSource::VIDEO);
        Config::setVideoPath(input);
        LOG_DEBUG("Input video: " + input);
    }

    // 3. open the frame source and getting frame correctly: send one frame per time to step 2
    FrameSource& frameSource = FrameSource::getInstance();
    if (!frameSource.initialize()) {
        LOG_ERROR("Failed to initialize frame source");
        return false;
    }

    return true;
}

void handleKeyboard(int key, Display& display) {
    switch (key) {
        case 'q':
        case 'Q':
            shouldExit = true;
            break;
        case 'c':
        case 'C':
            continuousMode = !continuousMode;
            LOG_DEBUG((continuousMode ? "Continuous mode ON" : "Frame-by-frame mode ON"));
            break;
        case ' ':
            if (!continuousMode) {
                LOG_DEBUG("Next frame");
            }
            break;
        case 'b':
        case 'B':
            display.toggleBoundingBoxes();
            LOG_DEBUG("Toggled bounding boxes visibility");
            break;
    }
}

/**
 * Usage: ./object-tracking [path_to_model] [--camera | <path_to_video>]
 */
int main(int argc, char* argv[]) {

    if (argc < 3) {
        LOG_ERROR("Usage: " + std::string(argv[0]) + " [<path_to_model>] [--camera | <path_to_video>]");
        return 1;
    }

    Logger::getInstance().setLogLevel(Logger::LogLevel::DEBUG);

    if (!initialization(argc, argv)) {
        return 1;
    }

    ThreadSafeQueue<Frame> preprocessQueue, trackingQueue, displayQueue;

    Preprocessor preprocessor(preprocessQueue, trackingQueue);
    Tracker tracker(trackingQueue, displayQueue);

    std::thread preprocessThread(&Preprocessor::run, &preprocessor);
    std::thread trackingThread(&Tracker::run, &tracker);

    FrameSource& frameSource = FrameSource::getInstance();
    Display display;

    Frame currentFrame;
    bool newFrameProcessed = false;

    auto processFrameFunc = [&]() {
        if (frameSource.getNextFrame(currentFrame)) {
            preprocessQueue.push(std::move(currentFrame));
            newFrameProcessed = true;
        } else {
            shouldExit = true;
        }
    };

    while (!shouldExit) {
        if (continuousMode || !newFrameProcessed) {
            processFrameFunc();
        }

        Frame processedFrame;
        if (displayQueue.pop(processedFrame)) {
            display.showFrame(processedFrame);
            newFrameProcessed = false;

            int key = cv::waitKey(1);
            handleKeyboard(key, display);

            if (!continuousMode) {
                // In frame-by-frame mode, wait for user input
                while (!shouldExit && !continuousMode) {
                    key = cv::waitKey(0);
                    handleKeyboard(key, display);
                    if (key == ' ') break;  // Space key to advance to next frame
                }
            }
        }

        // Check if we should exit
        if (shouldExit) break;
    }

    shouldExit = true;
    // Push empty frames to unblock queues
    preprocessQueue.push(Frame());
    trackingQueue.push(Frame());
    displayQueue.push(Frame());

    preprocessThread.join();
    trackingThread.join();

    return 0;
}