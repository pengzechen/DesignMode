
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>
#include <chrono>

using Matrix = std::vector<std::vector<double>>;

// ------------------- 线程安全队列 --------------------
template<typename T>
class ThreadSafeQueue {
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable cv;

public:
    void push(const T& val) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            queue.push(val);
        }
        cv.notify_one();
    }

    T wait_and_pop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !queue.empty(); });
        T val = queue.front();
        queue.pop();
        return val;
    }
};

// ------------------- 抽象观察者 --------------------
class IMatrixObserver {
public:
    virtual void submitMatrix(std::shared_ptr<Matrix> mat) = 0;
    virtual void stop() = 0;
    virtual ~IMatrixObserver() = default;
};

// ------------------- 日志系统 --------------------
class LoggerSystem : public IMatrixObserver {
    ThreadSafeQueue<std::shared_ptr<Matrix>> queue;
    std::atomic<bool> running{true};
    std::thread worker;

public:
    LoggerSystem() {
        worker = std::thread([this]() {
            while (running) {
                auto mat = queue.wait_and_pop();
                if (mat->empty()) continue; // 忽略 dummy 空矩阵

                std::cout << "[Logger] Received matrix: "
                        << mat->size() << "x" << (*mat)[0].size() << "\n";
            }
        });

    }

    void submitMatrix(std::shared_ptr<Matrix> mat) override {
        queue.push(mat);
    }

    void stop() override {
        running = false;
        queue.push(std::make_shared<Matrix>()); // dummy to unblock
        if (worker.joinable()) worker.join();
    }
};

// ------------------- 渲染系统 --------------------
class RenderSystem : public IMatrixObserver {
    ThreadSafeQueue<std::shared_ptr<Matrix>> queue;
    std::atomic<bool> running{true};
    std::thread worker;

public:
    RenderSystem() {
        worker = std::thread([this]() {
            while (running) {
                auto mat = queue.wait_and_pop();
                if (mat->empty()) continue;

                std::cout << "[Render] Rendering preview... ";
                std::this_thread::sleep_for(std::chrono::milliseconds(200)); // 模拟耗时
                std::cout << "Top-left = " << (*mat)[0][0] << "\n";
            }
        });
    }

    void submitMatrix(std::shared_ptr<Matrix> mat) override {
        queue.push(mat);
    }

    void stop() override {
        running = false;
        queue.push(std::make_shared<Matrix>()); // dummy
        if (worker.joinable()) worker.join();
    }
};

// ------------------- 计算系统 --------------------
class ComputeSystem : public IMatrixObserver {
    ThreadSafeQueue<std::shared_ptr<Matrix>> queue;
    std::atomic<bool> running{true};
    std::thread worker;

public:
    ComputeSystem() {
        worker = std::thread([this]() {
            while (running) {
                auto mat = queue.wait_and_pop();
                if (mat->empty()) continue;

                double sum = 0;
                for (const auto& row : *mat)
                    for (double val : row)
                        sum += val;

                std::cout << "[Compute] Matrix sum = " << sum << "\n";
            }
        });
    }

    void submitMatrix(std::shared_ptr<Matrix> mat) override {
        queue.push(mat);
    }

    void stop() override {
        running = false;
        queue.push(std::make_shared<Matrix>()); // dummy
        if (worker.joinable()) worker.join();
    }
};

// ------------------- 被观察者 --------------------
class MatrixGenerator {
    std::vector<IMatrixObserver*> observers;

public:
    void addObserver(IMatrixObserver* obs) {
        observers.push_back(obs);
    }

    void generateMatrix(size_t rows, size_t cols) {
        auto mat = std::make_shared<Matrix>(rows, std::vector<double>(cols, 0.0));
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                (*mat)[i][j] = static_cast<double>(i * cols + j);

        // std::cout << "[MatrixGenerator] Generated matrix " << rows << "x" << cols << "\n";

        for (auto* obs : observers)
            obs->submitMatrix(mat); // 异步传递引用
    }
};

// ------------------- 主程序 --------------------
int main() {
    LoggerSystem logger;
    RenderSystem renderer;
    ComputeSystem calculator;

    MatrixGenerator generator;
    generator.addObserver(&logger);
    generator.addObserver(&renderer);
    generator.addObserver(&calculator);

    for (int i=0; i<1000; i++)
        generator.generateMatrix(100, 100);

    std::this_thread::sleep_for(std::chrono::seconds(1)); // 等待各系统处理完

    logger.stop();
    renderer.stop();
    calculator.stop();

    std::cout << "ok" << std::endl;
    return 0;
}
