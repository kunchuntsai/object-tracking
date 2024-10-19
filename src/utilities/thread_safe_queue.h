/**
 * @file thread_safe_queue.h
 * @brief Thread-safe queue implementation
 */

#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

/**
 * @class ThreadSafeQueue
 * @brief A thread-safe implementation of a queue
 * @tparam T The type of elements stored in the queue
 */
template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable cond;

public:
    /**
     * @brief Push an item onto the queue
     * @param item The item to be pushed
     */
    void push(T item) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(std::move(item));
        cond.notify_one();
    }

    /**
     * @brief Pop an item from the queue
     * @param item Reference to store the popped item
     * @return true if an item was successfully popped, false otherwise
     */
    bool pop(T& item) {
        std::unique_lock<std::mutex> lock(mutex);
        cond.wait(lock, [this] { return !queue.empty(); });
        item = std::move(queue.front());
        queue.pop();
        return true;
    }
};