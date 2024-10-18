/**
 * @file preprocessor.h
 * @brief Header file for the Preprocessor class, responsible for preparing frames for inference.
 * @author Your Name
 * @date YYYY-MM-DD
 */

#pragma once

#include "frame.h"
#include "thread_safe_queue.h"
#include <onnxruntime_cxx_api.h>
#include <vector>

/**
 * @class Preprocessor
 * @brief Handles the preprocessing of frames before they are passed to the ONNX model.
 */
class Preprocessor {
public:
    /**
     * @brief Constructor for the Preprocessor class.
     * @param input Reference to the input queue of frames to be processed.
     * @param output Reference to the output queue where processed frames will be placed.
     * @param memory_info ONNX Runtime memory information.
     * @param input_node_dims Dimensions of the input node for the ONNX model.
     */
    Preprocessor(ThreadSafeQueue<Frame>& input, ThreadSafeQueue<Frame>& output, 
                 const Ort::MemoryInfo& memory_info, const std::vector<int64_t>& input_node_dims);

    /**
     * @brief Main processing loop for the Preprocessor.
     *
     * This method continuously takes frames from the input queue, preprocesses them,
     * and places the results in the output queue.
     */
    void run();

private:
    ThreadSafeQueue<Frame>& inputQueue; ///< Reference to the input queue
    ThreadSafeQueue<Frame>& outputQueue; ///< Reference to the output queue
    const Ort::MemoryInfo& memory_info; ///< ONNX Runtime memory information
    const std::vector<int64_t>& input_node_dims; ///< Dimensions of the ONNX model input node
};