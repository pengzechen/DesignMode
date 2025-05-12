#include <iostream>
#include <vector>
#include <memory>
#include <string>

// 假设矩阵就是一个二维vector
using Matrix = std::vector<std::vector<double>>;

// 观察者接口
class IMatrixObserver {
public:
    virtual void onMatrixGenerated(const Matrix& mat) = 0;
    virtual ~IMatrixObserver() = default;
};

// 被观察者（Subject）
class MatrixGenerator {
    std::vector<IMatrixObserver*> observers;
public:
    void addObserver(IMatrixObserver* obs) {
        observers.push_back(obs);
    }

    void generateMatrix(size_t rows, size_t cols) {
        Matrix mat(rows, std::vector<double>(cols, 0.0));

        // 简单填充数据
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                mat[i][j] = static_cast<double>(i * cols + j);

        std::cout << "[MatrixGenerator] Matrix generated: " << rows << "x" << cols << "\n";

        notifyObservers(mat);
    }

private:
    void notifyObservers(const Matrix& mat) {
        for (auto* obs : observers) {
            obs->onMatrixGenerated(mat);
        }
    }
};

// 日志系统
class LoggerSystem : public IMatrixObserver {
public:
    void onMatrixGenerated(const Matrix& mat) override {
        std::cout << "[LoggerSystem] Matrix of size "
                  << mat.size() << "x" << mat[0].size() << " received.\n";
    }
};

// 渲染系统
class RenderSystem : public IMatrixObserver {
public:
    void onMatrixGenerated(const Matrix& mat) override {
        std::cout << "[RenderSystem] Visualizing matrix preview...\n";
        std::cout << "  [0][0] = " << mat[0][0] << ", [0][1] = " << mat[0][1] << "\n";
    }
};

// 计算系统
class ComputeSystem : public IMatrixObserver {
public:
    void onMatrixGenerated(const Matrix& mat) override {
        double sum = 0;
        for (const auto& row : mat)
            for (double v : row)
                sum += v;
        std::cout << "[ComputeSystem] Matrix sum = " << sum << "\n";
    }
};

// ------------------- Main ---------------------
int main() {
    MatrixGenerator generator;

    LoggerSystem logger;
    RenderSystem renderer;
    ComputeSystem calculator;

    generator.addObserver(&logger);
    generator.addObserver(&renderer);
    generator.addObserver(&calculator);

    generator.generateMatrix(3, 3);  // 模拟生成一个 3x3 矩阵
    return 0;
}
