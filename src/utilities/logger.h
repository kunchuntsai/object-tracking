#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <mutex>
#include <string>

class Logger {
public:
    enum LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void logMessage(const std::string& message, LogLevel level) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (level >= currentLevel_) {
            std::cout << "[" << getLogLevelString(level) << "] " << message << std::endl;
        }
    }

    void setLogLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(mutex_);
        currentLevel_ = level;
    }

    LogLevel getLogLevel() const {
        return currentLevel_;
    }

    // Delete copy constructor and assignment
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Delete move constructor and assignment
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

private:
    Logger() : currentLevel_(LogLevel::INFO) {}

    std::string getLogLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }

    LogLevel currentLevel_;
    mutable std::mutex mutex_;
};

// Convenience macros for logging
#define LOG_DEBUG(message) Logger::getInstance().logMessage(message, Logger::LogLevel::DEBUG)
#define LOG_INFO(message) Logger::getInstance().logMessage(message, Logger::LogLevel::INFO)
#define LOG_WARNING(message) Logger::getInstance().logMessage(message, Logger::LogLevel::WARNING)
#define LOG_ERROR(message) Logger::getInstance().logMessage(message, Logger::LogLevel::ERROR)

#endif // LOGGER_HPP