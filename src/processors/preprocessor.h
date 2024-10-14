#pragma once

#include "frame.h"
#include "thread_safe_queue.h"


class Preprocessor {
public:
    Preprocessor(ThreadSafeQueue<Frame>& input, ThreadSafeQueue<Frame>& output);
    void run();

private:
    ThreadSafeQueue<Frame>& inputQueue;
    ThreadSafeQueue<Frame>& outputQueue;
};