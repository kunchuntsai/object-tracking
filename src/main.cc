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
    //if (!ONNXModel::loadModel("path/to/model.onnx")) {
    if (false) { //ONNX_TODO
        LOG_ERROR("Failed to load ONNX model");
        return false;
    }

    // 2. read config: init logger, check if the frame source is from video or camera feed, model parameters
    Logger::getInstance().setLogLevel(Logger::LogLevel::DEBUG);

    if (argc < 2) {
        LOG_ERROR("Usage: " + std::string(argv[0]) + " [--camera | <path_to_video>]");
        return false;
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

    // 3. open the frame source and getting frame correctly: send one frame per time to step 2
    FrameSource& frameSource = FrameSource::getInstance();
    if (!frameSource.initialize()) {
        LOG_ERROR("Failed to initialize frame source");
        return false;
    }

    return true;
}

void handleKeyboard(int key) {
    switch (key) {
        case 'q':
        case 'Q':
            shouldExit = true;
            break;
        case 'c':
        case 'C':
            continuousMode = !continuousMode;
            LOG_DEBUG((continuousMode ? "Continuous mode ON" : "Frame-by-frame mode ON"));
            //std::cout << (continuousMode ? "Continuous mode ON" : "Frame-by-frame mode ON") << std::endl;
            break;
        case ' ':
            if (!continuousMode) {
                LOG_DEBUG("Next frame");
                // std::cout << "Next frame" << std::endl;
            }
            break;
    }
}

int main(int argc, char* argv[]) {
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
            handleKeyboard(key);

            if (!continuousMode) {
                // In frame-by-frame mode, wait for user input
                while (!shouldExit && !continuousMode) {
                    key = cv::waitKey(0);
                    handleKeyboard(key);
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