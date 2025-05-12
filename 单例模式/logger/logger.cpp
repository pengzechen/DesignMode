#include "logger.hpp"
#include <iostream>
#include <ctime>
#include <chrono>

Logger::Logger() : exitFlag(false), minLevel(DEBUG) {
    logfile.open("app.log", std::ios_base::app);  // 打开日志文件
    if (!logfile.is_open()) {
        std::cerr << "Logger: Failed to open log file." << std::endl;
    }
    workerThread = std::thread(&Logger::processQueue, this);  // 启动后台线程
}

Logger::~Logger() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        exitFlag = true;  // 设置退出标志
    }
    queueCond.notify_all();
    if (workerThread.joinable()) {
        workerThread.join();  // 等待后台线程结束
    }
    if (logfile.is_open()) {
        logfile.close();  // 关闭文件
    }
}

Logger& Logger::getInstance() {
    static Logger instance;  // 静态局部变量，保证单例
    return instance;
}

void Logger::setLevel(Level level) {
    minLevel.store(level);  // 设置最低日志级别
}

void Logger::log(Level level, const std::string& message) {
    if (level < minLevel.load()) return;  // 如果日志级别低于设定的最低级别，则不记录日志

    std::string formatted = formatMessage(level, message);
    {
        std::lock_guard<std::mutex> lock(queueMutex);  // 加锁
        logQueue.push(formatted);  // 将格式化后的日志添加到队列中
    }
    queueCond.notify_one();  // 通知后台线程
}

void Logger::processQueue() {
    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCond.wait(lock, [this]() {
            return !logQueue.empty() || exitFlag;
        });

        while (!logQueue.empty()) {
            logfile << logQueue.front() << std::endl;  // 将队列中的日志写入文件
            logQueue.pop();  // 从队列中移除
        }

        if (exitFlag && logQueue.empty()) {
            break;  // 如果退出标志为真并且队列为空，则退出线程
        }
    }
}

std::string Logger::formatMessage(Level level, const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    char buf[26];
    ctime_r(&time, buf);
    buf[24] = '\0';  // 移除换行符

    return "[" + std::string(buf) + "] [" + levelToString(level) + "] " + message;
}

std::string Logger::levelToString(Level level) {
    switch (level) {
        case DEBUG:   return "DEBUG";
        case INFO:    return "INFO";
        case WARNING: return "WARNING";
        case ERROR:   return "ERROR";
        default:      return "UNKNOWN";
    }
}
