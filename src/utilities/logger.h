#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <sstream>

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

    void logMessage(const std::string& message, LogLevel level) {
        if (static_cast<int>(level) & currentLogLevel) {
            std::cout << getLevelString(level) << ": " << message << std::endl;
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
#if LOG_LEVEL & LOG_LEVEL_DEBUG
    #define LOG_DEBUG(message) do { std::ostringstream ss; ss << message; Logger::getInstance().logMessage(ss.str(), Logger::LogLevel::DEBUG); } while(0)
#else
    #define LOG_DEBUG(message) do {} while(0)
#endif

#if LOG_LEVEL & LOG_LEVEL_INFO
    #define LOG_INFO(message) do { std::ostringstream ss; ss << message; Logger::getInstance().logMessage(ss.str(), Logger::LogLevel::INFO); } while(0)
#else
    #define LOG_INFO(message) do {} while(0)
#endif

#if LOG_LEVEL & LOG_LEVEL_WARNING
    #define LOG_WARNING(message) do { std::ostringstream ss; ss << message; Logger::getInstance().logMessage(ss.str(), Logger::LogLevel::WARNING); } while(0)
#else
    #define LOG_WARNING(message) do {} while(0)
#endif

#if LOG_LEVEL & LOG_LEVEL_ERROR
    #define LOG_ERROR(message) do { std::ostringstream ss; ss << message; Logger::getInstance().logMessage(ss.str(), Logger::LogLevel::ERROR); } while(0)
#else
    #define LOG_ERROR(message) do {} while(0)
#endif

#endif // LOGGER_H