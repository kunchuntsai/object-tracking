#include "preprocessor.h"
#include "image_process.h"
#include "logger.h"
#include "config.h"

extern std::atomic<bool> shouldExit;

Preprocessor::Preprocessor(ThreadSafeQueue<Frame>& input, ThreadSafeQueue<Frame>& output,
                           const Ort::MemoryInfo& memory_info, const std::vector<int64_t>& input_node_dims)
    : inputQueue(input), outputQueue(output), memory_info(memory_info), input_node_dims(input_node_dims) {

    LOG_INFO("ONNX model input dimensions: " + std::to_string(input_node_dims[0]) + "x" +
             std::to_string(input_node_dims[1]) + "x" + std::to_string(input_node_dims[2]) + "x" +
             std::to_string(input_node_dims[3]));
}

void Preprocessor::run() {
    // Assuming input_node_dims is in the format {batch_size, channels, height, width}
    int inputWidth = input_node_dims[3];
    int inputHeight = input_node_dims[2];

    while (!shouldExit) {
        Frame frame;
        if (inputQueue.pop(frame)) {
            if (frame.original.empty()) {
                LOG_ERROR("Frame is empty");
                continue;
            }
            frame.processed = ImageProcessor::processFrame(frame.original, inputWidth, inputHeight);

            if (frame.processed.cols != inputWidth || frame.processed.rows != inputHeight) {
                LOG_ERROR("Processed frame dimensions (" + std::to_string(frame.processed.cols) + "x" +
                          std::to_string(frame.processed.rows) + ") do not match model input dimensions (" +
                          std::to_string(inputWidth) + "x" + std::to_string(inputHeight) + ")");
                continue;
            }

            // Preprocess for ONNX
            frame.onnx_input = ImageProcessor::preprocessForONNX(frame.processed, memory_info, input_node_dims);

            outputQueue.push(std::move(frame));
        }
    }
}