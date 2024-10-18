#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <sstream>
#include <cstdarg>

// Define log levels as bitwise flags
#define LOG_LEVEL_NONE    0
#define LOG_LEVEL_ERROR   1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_INFO    4
#define LOG_LEVEL_DEBUG   8

// Set the desired log level here (can be overridden by compiler flags)
#ifndef LOG_LEVEL
#define LOG_LEVEL (LOG_LEVEL_ERROR | LOG_LEVEL_WARNING | LOG_LEVEL_INFO)
#endif

class Logger {
public:
    enum class LogLevel {
        NONE = LOG_LEVEL_NONE,
        ERROR = LOG_LEVEL_ERROR,
        WARNING = LOG_LEVEL_WARNING,
        INFO = LOG_LEVEL_INFO,
        DEBUG = LOG_LEVEL_DEBUG
    };

    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void setLogLevel(int level) {
        currentLogLevel = level;
    }

    void logMessage(const char* format, LogLevel level, ...) {
        if (static_cast<int>(level) & currentLogLevel) {
            va_list args;
            va_start(args, level);

            char buffer[1024];
            vsnprintf(buffer, sizeof(buffer), format, args);

            va_end(args);

            std::cout << getLevelString(level) << ": " << buffer << std::endl;
        }
    }

private:
    Logger() : currentLogLevel(LOG_LEVEL) {}
    int currentLogLevel;

    std::string getLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }
};

// Macros for logging
#define LOG_DEBUG(format, ...) Logger::getInstance().logMessage(format, Logger::LogLevel::DEBUG, ##__VA_ARGS__)
#define LOG_INFO(format, ...) Logger::getInstance().logMessage(format, Logger::LogLevel::INFO, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) Logger::getInstance().logMessage(format, Logger::LogLevel::WARNING, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) Logger::getInstance().logMessage(format, Logger::LogLevel::ERROR, ##__VA_ARGS__)

#endif // LOGGER_H