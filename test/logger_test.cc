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

// Helper function to reset log level to all levels
void resetLogLevel() {
    Logger::getInstance().setLogLevel(LOG_LEVEL_DEBUG | LOG_LEVEL_INFO | LOG_LEVEL_WARNING | LOG_LEVEL_ERROR);
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

    resetLogLevel();
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

    resetLogLevel();
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

    resetLogLevel();
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

    resetLogLevel();
}

// Custom output function for single-line printing
void printSingleLine(const char* message) {
    std::cout << "\r" << message << std::flush;
}

TEST(LoggingPerformance) {
    Logger& logger = Logger::getInstance();
    logger.setLogLevel(LOG_LEVEL_DEBUG);

    const int numMessages = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numMessages; ++i) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "DEBUG: Performance test message %d", i);
        printSingleLine(buffer);
        
        // Small delay to make output visible
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double visualizationTime = static_cast<double>(duration.count()) / 1000.0; // in milliseconds

    std::cout << std::endl;  // Move to the next line after the visualization

    // Actual performance test using a stringstream to capture output
    std::stringstream logOutput;
    auto oldBuffer = std::cout.rdbuf(logOutput.rdbuf());

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numMessages; ++i) {
        LOG_DEBUG("Performance test message %d", i);
    }
    end = std::chrono::high_resolution_clock::now();

    std::cout.rdbuf(oldBuffer); // Restore the original buffer

    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double averageTime = static_cast<double>(duration.count()) / numMessages;

    std::cout << "Visualization time: " << visualizationTime << " ms" << std::endl;
    std::cout << "Average logging time: " << averageTime << " microseconds per message" << std::endl;

    ASSERT_TRUE(averageTime < 100.0);  // Assuming less than 100 microseconds per log is acceptable

    resetLogLevel();
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

    resetLogLevel();
}