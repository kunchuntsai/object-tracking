/**
 * @file logger.h
 * @brief Logging utility for the application
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <sstream>
#include <cstdarg>

/// @brief Log level: No logging
#define LOG_LV_NONE    0
/// @brief Log level: Error messages only
#define LOG_LV_ERROR   1
/// @brief Log level: Warning and error messages
#define LOG_LV_WARNING 2
/// @brief Log level: Info, warning, and error messages
#define LOG_LV_INFO    4
/// @brief Log level: All messages including debug
#define LOG_LV_DEBUG   8

/**
 * @brief Set the desired log level here (can be overridden by compiler flags)
 */
#ifndef LOG_LEVEL
#define LOG_LEVEL (LOG_LV_ERROR | LOG_LV_WARNING | LOG_LV_INFO)
#endif

/**
 * @class Logger
 * @brief Singleton class for logging messages
 */
class Logger {
public:
    /**
     * @enum LogLevel
     * @brief Enumeration of log levels
     */
    enum class LogLevel {
        NONE = LOG_LV_NONE,     /**< No logging */
        ERROR = LOG_LV_ERROR,   /**< Error messages only */
        WARNING = LOG_LV_WARNING, /**< Warning and error messages */
        INFO = LOG_LV_INFO,     /**< Info, warning, and error messages */
        DEBUG = LOG_LV_DEBUG    /**< All messages including debug */
    };

    /**
     * @brief Get the singleton instance of the Logger
     * @return Reference to the Logger instance
     */
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    /**
     * @brief Set the current log level
     * @param level The log level to set
     */
    void setLogLevel(int level) {
        currentLogLevel = level;
    }

    /**
     * @brief Log a message if it meets the current log level
     * @param format The format string for the message
     * @param level The log level of the message
     * @param ... Additional arguments for formatting
     */
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

    /**
     * @brief Get the string representation of a log level
     * @param level The log level
     * @return String representation of the log level
     */
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

/// @brief Macro for logging debug messages
#define LOG_DEBUG(format, ...) Logger::getInstance().logMessage(format, Logger::LogLevel::DEBUG, ##__VA_ARGS__)
/// @brief Macro for logging info messages
#define LOG_INFO(format, ...) Logger::getInstance().logMessage(format, Logger::LogLevel::INFO, ##__VA_ARGS__)
/// @brief Macro for logging warning messages
#define LOG_WARNING(format, ...) Logger::getInstance().logMessage(format, Logger::LogLevel::WARNING, ##__VA_ARGS__)
/// @brief Macro for logging error messages
#define LOG_ERROR(format, ...) Logger::getInstance().logMessage(format, Logger::LogLevel::ERROR, ##__VA_ARGS__)

#endif // LOGGER_H