#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable cond;

public:
    void push(T item) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(std::move(item));
        cond.notify_one();
    }

    bool pop(T& item) {
        std::unique_lock<std::mutex> lock(mutex);
        cond.wait(lock, [this] { return !queue.empty(); });
        item = std::move(queue.front());
        queue.pop();
        return true;
    }
};