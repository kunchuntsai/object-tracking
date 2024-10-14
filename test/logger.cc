#include "logger.h"
#include <thread>
#include <chrono>

void runLoggerTests() {
    Logger& logger = Logger::getInstance();

    // Test different log levels
    logger.setLogLevel(Logger::LogLevel::DEBUG);
    LOG_DEBUG("This is a debug message");
    LOG_INFO("This is an info message");
    LOG_WARNING("This is a warning message");
    LOG_ERROR("This is an error message");

    // Test changing log levels
    logger.setLogLevel(Logger::LogLevel::WARNING);
    LOG_DEBUG("This debug message should not appear");
    LOG_INFO("This info message should not appear");
    LOG_WARNING("This warning message should appear");
    LOG_ERROR("This error message should appear");

    // Test thread safety
    std::thread t1([]() {
        for (int i = 0; i < 5; ++i) {
            LOG_INFO("Thread 1: Message " + std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    std::thread t2([]() {
        for (int i = 0; i < 5; ++i) {
            LOG_INFO("Thread 2: Message " + std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    t1.join();
    t2.join();

    // Test logging performance
    const int numMessages = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numMessages; ++i) {
        LOG_DEBUG("Performance test message " + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double averageTime = static_cast<double>(duration.count()) / numMessages;

    std::string message = "Average time per log message: " + std::to_string(averageTime) + " microseconds";
    LOG_INFO(message);
}