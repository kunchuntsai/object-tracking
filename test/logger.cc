#include "logger.h"
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>

void runLoggerTests() {
    Logger& logger = Logger::getInstance();

    // Test individual log levels
    logger.setLogLevel(LOG_LEVEL_DEBUG | LOG_LEVEL_INFO | LOG_LEVEL_WARNING | LOG_LEVEL_ERROR);
    LOG_DEBUG("This is a debug message");
    LOG_INFO("This is an info message");
    LOG_WARNING("This is a warning message");
    LOG_ERROR("This is an error message");

    // Test changing log levels
    logger.setLogLevel(LOG_LEVEL_WARNING | LOG_LEVEL_ERROR);
    LOG_DEBUG("This debug message should not appear");
    LOG_INFO("This info message should not appear");
    LOG_WARNING("This warning message should appear");
    LOG_ERROR("This error message should appear");

    // Test combining log levels
    logger.setLogLevel(LOG_LEVEL_DEBUG | LOG_LEVEL_ERROR);
    LOG_DEBUG("This debug message should appear");
    LOG_INFO("This info message should not appear");
    LOG_WARNING("This warning message should not appear");
    LOG_ERROR("This error message should appear");

    // Test thread safety
    logger.setLogLevel(LOG_LEVEL_INFO);
    std::vector<std::thread> threads;
    for (int t = 0; t < 5; ++t) {
        threads.emplace_back([t]() {
            for (int i = 0; i < 5; ++i) {
                LOG_INFO("Thread " + std::to_string(t) + ": Message " + std::to_string(i));
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    std::for_each(threads.begin(), threads.end(), [](std::thread &t) { t.join(); });

    // Test logging performance
    const int numMessages = 1000;
    logger.setLogLevel(LOG_LEVEL_DEBUG);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numMessages; ++i) {
        LOG_DEBUG("Performance test message " + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double averageTime = static_cast<double>(duration.count()) / numMessages;

    logger.setLogLevel(LOG_LEVEL_INFO);
    LOG_INFO("Average time per log message: " + std::to_string(averageTime) + " microseconds");

    // Test disabling all logs
    logger.setLogLevel(LOG_LEVEL_NONE);
    LOG_DEBUG("This debug message should not appear");
    LOG_INFO("This info message should not appear");
    LOG_WARNING("This warning message should not appear");
    LOG_ERROR("This error message should not appear");
}