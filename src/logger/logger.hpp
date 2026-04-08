#pragma once

#include <string>
#include <functional>
#include <format>
#include <chrono>
#include <ctime>
#include <iostream>
#include <utility>
#include <arpa/inet.h>
#include <format>

enum LogLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

inline std::string LogLevelToString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::DEBUG:
        return "DEBUG";
    case LogLevel::INFO:
        return "INFO";
    case LogLevel::WARNING:
        return "WARNING";
    case LogLevel::ERROR:
        return "ERROR";
    default:
        return "";
    }
}

typedef std::function<std::string(const std::string &, LogLevel, std::string name)> LoggerFormatter;
typedef std::function<void(std::string &)> LoggerOutput;

inline const LoggerOutput consoleOutputCallback = [](std::string &message)
{
    std::cout << message << std::endl;
};

inline const LoggerFormatter baseFormatterCallback = [](const std::string &message, LogLevel level, std::string name) -> std::string
{
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now;
    localtime_r(&time_t_now, &tm_now);

    char time_buffer[80];
    std::strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", &tm_now);

    return std::format("[{}] [{}] - {} - {}", LogLevelToString(level), name, time_buffer, message);
};

class BaseLogger
{
public:
    virtual void log(const std::string &message, LogLevel level = LogLevel::INFO);
    virtual void debug(const std::string &message);
    virtual void info(const std::string &message);
    virtual void warning(const std::string &message);
    virtual void error(const std::string &message);

    void setFormatter(const LoggerFormatter &formatter);
    void setOutput(const LoggerOutput &output);
    BaseLogger(LogLevel level);
    void setName(const std::string &name);

private:
    LoggerFormatter formatter = baseFormatterCallback;
    LoggerOutput output = consoleOutputCallback;
    std::string name = "Logger";
    LogLevel level = LogLevel::INFO;
};

class NullLogger : public BaseLogger
{
public:
    NullLogger() : BaseLogger(LogLevel::DEBUG) {}

    inline void log(const std::string &message, LogLevel level = LogLevel::INFO) override {};
    inline void info(const std::string &) override {}
    inline void debug(const std::string &) override {}
    inline void warning(const std::string &message) override {};
    inline void error(const std::string &) override {}
};