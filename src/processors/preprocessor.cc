#include "preprocessor.h"
#include "image_process.h"
#include "logger.h"

extern std::atomic<bool> shouldExit;


Preprocessor::Preprocessor(ThreadSafeQueue<Frame>& input, ThreadSafeQueue<Frame>& output)
    : inputQueue(input), outputQueue(output) {}

void Preprocessor::run() {
    while (!shouldExit) {
        Frame frame;
        if (inputQueue.pop(frame)) {
            if (frame.original.empty()) { LOG_ERROR("Frame is empty"); continue; }
            frame.processed = ImageProcessor::processFrame(frame.original, 640, 480);
            outputQueue.push(std::move(frame));
        }
    }
}