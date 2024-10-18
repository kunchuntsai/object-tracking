#include "preprocessor.h"
#include "image_process.h"
#include "logger.h"
#include "config.h"
#include <chrono>

extern std::atomic<bool> shouldExit;
extern std::atomic<long long> totalPreprocessTime;

Preprocessor::Preprocessor(ThreadSafeQueue<Frame>& input, ThreadSafeQueue<Frame>& output,
                           const Ort::MemoryInfo& memory_info, const std::vector<int64_t>& input_node_dims)
    : inputQueue(input), outputQueue(output), memory_info(memory_info), input_node_dims(input_node_dims) {

    LOG_INFO("[Preproc] ONNX model input dimensions: %ldx%ldx%ldx%ld",
             input_node_dims[0], input_node_dims[1], input_node_dims[2], input_node_dims[3]);
}

void Preprocessor::run() {
    int inputWidth = input_node_dims[3];
    int inputHeight = input_node_dims[2];
    LOG_DEBUG("[Preproc] Input width %d, height %d", inputWidth, inputHeight);

    while (!shouldExit) {
        Frame frame;
        if (inputQueue.pop(frame)) {
            auto start = std::chrono::high_resolution_clock::now();

            if (frame.original.empty()) {
                LOG_ERROR("[Preproc] Frame is empty");
                continue;
            }
            frame.processed = ImageProcessor::processFrame(frame.original, inputWidth, inputHeight);

            if (frame.processed.cols != inputWidth || frame.processed.rows != inputHeight) {
                LOG_ERROR("[Preproc] Processed frame dimensions (%dx%d) do not match model input dimensions (%dx%d)",
                          frame.processed.cols, frame.processed.rows, inputWidth, inputHeight);
                continue;
            }

            // Preprocess for ONNX
            frame.onnx_input = ImageProcessor::preprocessForONNX(frame.processed, memory_info, input_node_dims);

            outputQueue.push(std::move(frame));

            auto end = std::chrono::high_resolution_clock::now();
            totalPreprocessTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            LOG_DEBUG("[Preproc] Finished preproc and pushed to output queue");
        }
    }
}