#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <ctime>
#include <atomic>

class Logger::Impl {
public:
    Impl()
        : exitFlag(false), minLevel(DEBUG) {
        logfile.open("app.log", std::ios_base::app);
        if (!logfile.is_open()) {
            std::cerr << "Logger: Failed to open log file." << std::endl;
        }
        workerThread = std::thread(&Impl::processQueue, this);
    }

    ~Impl() {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            exitFlag = true;
        }
        queueCond.notify_all();
        if (workerThread.joinable()) {
            workerThread.join();
        }
        if (logfile.is_open()) {
            logfile.close();
        }
    }

    void setLevel(Level level) {
        minLevel.store(level);
    }

    void log(Level level, const std::string& message) {
        if (level < minLevel.load()) return;

        std::string formatted = formatMessage(level, message);
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            logQueue.push(formatted);
        }
        queueCond.notify_one();
    }

private:
    std::ofstream logfile;
    std::mutex queueMutex;
    std::condition_variable queueCond;
    std::queue<std::string> logQueue;
    std::thread workerThread;
    std::atomic<bool> exitFlag;
    std::atomic<Logger::Level> minLevel;

    void processQueue() {
        while (true) {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCond.wait(lock, [this]() {
                return !logQueue.empty() || exitFlag;
            });

            while (!logQueue.empty()) {
                logfile << logQueue.front() << std::endl;
                logQueue.pop();
            }

            if (exitFlag && logQueue.empty()) {
                break;
            }
        }
    }

    std::string formatMessage(Level level, const std::string& message) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        char buf[26];
        ctime_r(&time, buf);
        buf[24] = '\0'; // Remove newline

        return "[" + std::string(buf) + "] [" + levelToString(level) + "] " + message;
    }

    std::string levelToString(Level level) {
        switch (level) {
            case DEBUG:   return "DEBUG";
            case INFO:    return "INFO";
            case WARNING: return "WARNING";
            case ERROR:   return "ERROR";
            default:      return "UNKNOWN";
        }
    }

    friend class Logger;
};

// =========== Logger 外部接口实现 ===========

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    pImpl = new Impl();
}

Logger::~Logger() {
    delete pImpl;
}

void Logger::setLevel(Level level) {
    pImpl->setLevel(level);
}

void Logger::log(Level level, const std::string& message) {
    pImpl->log(level, message);
}
