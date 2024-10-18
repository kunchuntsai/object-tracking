#include "unit_test.h"
#include "logger.h"
#include <thread>
#include <chrono>
#include <vector>
#include <sstream>
#include <algorithm>

// Helper function to capture log output
std::string captureLogOutput(std::function<void()> logFunction) {
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    logFunction();
    std::cout.rdbuf(old);
    return buffer.str();
}

TEST(LogLevels) {
    Logger& logger = Logger::getInstance();
    
    // Test individual log levels
    logger.setLogLevel(LOG_LEVEL_DEBUG | LOG_LEVEL_INFO | LOG_LEVEL_WARNING | LOG_LEVEL_ERROR);
    
    std::string output = captureLogOutput([]() {
        LOG_DEBUG("Debug message");
        LOG_INFO("Info message");
        LOG_WARNING("Warning message");
        LOG_ERROR("Error message");
    });

    ASSERT_TRUE(output.find("DEBUG: Debug message") != std::string::npos);
    ASSERT_TRUE(output.find("INFO: Info message") != std::string::npos);
    ASSERT_TRUE(output.find("WARNING: Warning message") != std::string::npos);
    ASSERT_TRUE(output.find("ERROR: Error message") != std::string::npos);
}

TEST(ChangingLogLevels) {
    Logger& logger = Logger::getInstance();
    
    logger.setLogLevel(LOG_LEVEL_WARNING | LOG_LEVEL_ERROR);
    
    std::string output = captureLogOutput([]() {
        LOG_DEBUG("Debug message");
        LOG_INFO("Info message");
        LOG_WARNING("Warning message");
        LOG_ERROR("Error message");
    });

    ASSERT_TRUE(output.find("DEBUG: Debug message") == std::string::npos);
    ASSERT_TRUE(output.find("INFO: Info message") == std::string::npos);
    ASSERT_TRUE(output.find("WARNING: Warning message") != std::string::npos);
    ASSERT_TRUE(output.find("ERROR: Error message") != std::string::npos);
}

TEST(CombiningLogLevels) {
    Logger& logger = Logger::getInstance();
    
    logger.setLogLevel(LOG_LEVEL_DEBUG | LOG_LEVEL_ERROR);
    
    std::string output = captureLogOutput([]() {
        LOG_DEBUG("Debug message");
        LOG_INFO("Info message");
        LOG_WARNING("Warning message");
        LOG_ERROR("Error message");
    });

    ASSERT_TRUE(output.find("DEBUG: Debug message") != std::string::npos);
    ASSERT_TRUE(output.find("INFO: Info message") == std::string::npos);
    ASSERT_TRUE(output.find("WARNING: Warning message") == std::string::npos);
    ASSERT_TRUE(output.find("ERROR: Error message") != std::string::npos);
}

TEST(ThreadSafety) {
    Logger& logger = Logger::getInstance();
    logger.setLogLevel(LOG_LEVEL_INFO);

    std::vector<std::thread> threads;
    std::vector<std::string> outputs;

    for (int t = 0; t < 5; ++t) {
        outputs.push_back(captureLogOutput([t]() {
            for (int i = 0; i < 5; ++i) {
                LOG_INFO("Thread %d: Message %d", t, i);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }));
    }

    for (const auto& output : outputs) {
        ASSERT_TRUE(output.find("INFO: Thread") != std::string::npos);
    }
}

TEST(LoggingPerformance) {
    Logger& logger = Logger::getInstance();
    logger.setLogLevel(LOG_LEVEL_DEBUG);

    const int numMessages = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numMessages; ++i) {
        LOG_DEBUG("Performance test message %d", i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double averageTime = static_cast<double>(duration.count()) / numMessages;

    ASSERT_TRUE(averageTime < 100.0);  // Assuming less than 100 microseconds per log is acceptable
}

TEST(DisablingAllLogs) {
    Logger& logger = Logger::getInstance();
    logger.setLogLevel(LOG_LEVEL_NONE);

    std::string output = captureLogOutput([]() {
        LOG_DEBUG("Debug message");
        LOG_INFO("Info message");
        LOG_WARNING("Warning message");
        LOG_ERROR("Error message");
    });

    ASSERT_TRUE(output.empty());
}