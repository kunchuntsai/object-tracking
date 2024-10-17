#pragma once

#include "frame.h"
#include "thread_safe_queue.h"
#include <onnxruntime_cxx_api.h>

class Preprocessor {
public:
    Preprocessor(ThreadSafeQueue<Frame>& input, ThreadSafeQueue<Frame>& output, 
                 const Ort::MemoryInfo& memory_info, const std::vector<int64_t>& input_node_dims);
    void run();

private:
    ThreadSafeQueue<Frame>& inputQueue;
    ThreadSafeQueue<Frame>& outputQueue;
    const Ort::MemoryInfo& memory_info;
    const std::vector<int64_t>& input_node_dims;
};