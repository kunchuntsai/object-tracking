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

// Profiling variables
std::atomic<long long> totalMainTime(0);
std::atomic<long long> totalPreprocessTime(0);
std::atomic<long long> totalTrackerTime(0);
std::atomic<int> frameCount(0);

void printProfilingResults() {
    int frames = frameCount.load();
    if (frames == 0) return;

    double avgMainTime = static_cast<double>(totalMainTime.load()) / frames / 1e6;
    double avgPreprocessTime = static_cast<double>(totalPreprocessTime.load()) / frames / 1e6;
    double avgTrackerTime = static_cast<double>(totalTrackerTime.load()) / frames / 1e6;

    LOG_INFO("Profiling results (%d frames):", frames);
    LOG_INFO("   Main thread avg time: %.2f ms", avgMainTime);
    LOG_INFO("   Preprocessor avg time: %.2f ms", avgPreprocessTime);
    LOG_INFO("   Tracker avg time: %.2f ms", avgTrackerTime);
    LOG_INFO("   Total avg time per frame: %.2f ms", avgMainTime + avgPreprocessTime + avgTrackerTime);
    LOG_INFO("   Average FPS: %.2f", 1000.0 / (avgMainTime + avgPreprocessTime + avgTrackerTime));
}

bool initialization(const std::string& configPath) {
    // Set log level
    Logger::getInstance().setLogLevel(LOG_LEVEL_ERROR | LOG_LEVEL_WARNING | LOG_LEVEL_INFO);

    // Load configuration
    if (!Config::loadFromFile(configPath)) {
        LOG_ERROR("Failed to load configuration file");
        return false;
    }

    // Load ONNX model
    if (!ONNXModel::getInstance().loadModel(Config::getModelPath())) {
        LOG_ERROR("Failed to load ONNX model");
        return false;
    }

    // Frame source is already set by the config file

    // Initialize frame source
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
            LOG_DEBUG(continuousMode ? "Continuous mode ON" : "Frame-by-frame mode ON");
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
 * Usage: ./object-tracking <path_to_config_file>
 */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        LOG_ERROR("Usage: %s <path_to_config_file>", argv[0]);
        return 1;
    }

    std::string configPath = argv[1];

    if (!initialization(configPath)) {
        return 1;
    }

    ThreadSafeQueue<Frame> preprocessQueue, trackingQueue, displayQueue;

    ONNXModel& model = ONNXModel::getInstance();
    Preprocessor preprocessor(preprocessQueue, trackingQueue, model.getMemoryInfo(), model.getInputNodeDims());
    Tracker tracker(trackingQueue, displayQueue);

    std::thread preprocessThread(&Preprocessor::run, &preprocessor);
    std::thread trackingThread(&Tracker::run, &tracker);

    FrameSource& frameSource = FrameSource::getInstance();
    Display display;

    Frame currentFrame;
    bool newFrameProcessed = false;

    auto processFrameFunc = [&]() {
        auto start = std::chrono::high_resolution_clock::now();
        if (frameSource.getNextFrame(currentFrame)) {
            preprocessQueue.push(std::move(currentFrame));
            newFrameProcessed = true;
        } else {
            shouldExit = true;
        }
        auto end = std::chrono::high_resolution_clock::now();
        totalMainTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    };

    while (!shouldExit) {
        if (continuousMode || !newFrameProcessed) {
            processFrameFunc();
        }

        Frame processedFrame;
        if (displayQueue.pop(processedFrame)) {
            auto start = std::chrono::high_resolution_clock::now();
            display.showFrame(processedFrame);
            newFrameProcessed = false;

            int key = cv::waitKey(1);
            handleKeyboard(key, display);
            auto end = std::chrono::high_resolution_clock::now();
            totalMainTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            frameCount++;

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

    // Print profiling results
    printProfilingResults();

    return 0;
}