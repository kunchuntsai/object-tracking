#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <sstream>

class Logger {
public:
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void setLogLevel(LogLevel level) {
        currentLogLevel = level;
    }

    void logMessage(const std::string& message, LogLevel level) {
        if (level >= currentLogLevel) {
            std::cout << getLevelString(level) << ": " << message << std::endl;
        }
    }

private:
    Logger() : currentLogLevel(LogLevel::INFO) {}
    LogLevel currentLogLevel;

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

#define LOG_DEBUG(message) do { std::ostringstream ss; ss << message; Logger::getInstance().logMessage(ss.str(), Logger::LogLevel::DEBUG); } while(0)
#define LOG_INFO(message) do { std::ostringstream ss; ss << message; Logger::getInstance().logMessage(ss.str(), Logger::LogLevel::INFO); } while(0)
#define LOG_WARNING(message) do { std::ostringstream ss; ss << message; Logger::getInstance().logMessage(ss.str(), Logger::LogLevel::WARNING); } while(0)
#define LOG_ERROR(message) do { std::ostringstream ss; ss << message; Logger::getInstance().logMessage(ss.str(), Logger::LogLevel::ERROR); } while(0)

#endif // LOGGER_H