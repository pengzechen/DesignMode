#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>

class Logger {
public:
    enum Level { DEBUG, INFO, WARNING, ERROR };

    static Logger& getInstance();  // 获取单例实例

    void setLevel(Level level);    // 设置日志级别
    void log(Level level, const std::string& message);  // 记录日志

private:
    Logger();  // 构造函数
    ~Logger();  // 析构函数

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void processQueue();  // 后台线程处理日志队列
    std::string formatMessage(Level level, const std::string& message);  // 格式化日志信息
    std::string levelToString(Level level);  // 将日志级别转换为字符串

    std::ofstream logfile;  // 日志文件流
    std::mutex queueMutex;  // 锁
    std::condition_variable queueCond;  // 条件变量
    std::queue<std::string> logQueue;  // 日志队列
    std::thread workerThread;  // 后台线程
    bool exitFlag;  // 退出标志
    std::atomic<Level> minLevel;  // 最低日志级别
};
