#include "logger.hpp"

void BaseLogger::log(const std::string &message, LogLevel level)
{
    if (level < this->level)
        return;
    auto formattedMessage = this->formatter(message, level, this->name);
    this->output(formattedMessage);
}

void BaseLogger::debug(const std::string &message)
{
    this->log(message, LogLevel::DEBUG);
}

void BaseLogger::info(const std::string &message)
{
    this->log(message, LogLevel::INFO);
}

void BaseLogger::warning(const std::string &message)
{
    this->log(message, LogLevel::WARNING);
}

void BaseLogger::error(const std::string &message)
{
    this->log(message, LogLevel::ERROR);
}

void BaseLogger::setFormatter(const LoggerFormatter &formatter)
{
    this->formatter = formatter;
}

void BaseLogger::setOutput(const LoggerOutput &output)
{
    this->output = output;
}

BaseLogger::BaseLogger(LogLevel level) : level(level) {}

void BaseLogger::setName(const std::string &name)
{
    this->name = name;
}
