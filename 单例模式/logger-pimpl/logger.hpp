#pragma once
#include <string>

class Logger {
public:
    enum Level { DEBUG, INFO, WARNING, ERROR };

    static Logger& getInstance();

    void setLevel(Level level);
    void log(Level level, const std::string& message);

private:
    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void processQueue();
    std::string formatMessage(Level level, const std::string& message);
    std::string levelToString(Level level);

    class Impl;
    Impl* pImpl;
};
