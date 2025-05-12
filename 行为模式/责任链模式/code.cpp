#include <iostream>
#include <memory>

// 抽象处理者
class Handler {
protected:
    std::shared_ptr<Handler> next;

public:
    void setNext(std::shared_ptr<Handler> handler) {
        next = handler;
    }

    virtual void handleRequest(int days) {
        if (next) next->handleRequest(days);
        else std::cout << "No handler for " << days << " days.\n";
    }

    virtual ~Handler() = default;
};

// 具体处理者
class Manager : public Handler {
public:
    void handleRequest(int days) override {
        if (days <= 3)
            std::cout << "Manager approves " << days << " day(s).\n";
        else if (next)
            next->handleRequest(days);
    }
};

class Director : public Handler {
public:
    void handleRequest(int days) override {
        if (days <= 7)
            std::cout << "Director approves " << days << " day(s).\n";
        else if (next)
            next->handleRequest(days);
    }
};

class HR : public Handler {
public:
    void handleRequest(int days) override {
        std::cout << "HR approves " << days << " day(s).\n";
    }
};

// 主程序
int main() {
    auto manager = std::make_shared<Manager>();
    auto director = std::make_shared<Director>();
    auto hr = std::make_shared<HR>();

    manager->setNext(director);
    director->setNext(hr);

    int requests[] = {2, 5, 10};

    for (int days : requests) {
        std::cout << "Request for " << days << " day(s): ";
        manager->handleRequest(days);
    }

    return 0;
}
