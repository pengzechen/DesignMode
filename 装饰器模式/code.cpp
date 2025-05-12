

#include <iostream>
#include <string>

class Beverage {
public:
    virtual std::string getDescription() const = 0; // 获取描述
    virtual double cost() const = 0;  // 获取价格
    virtual ~Beverage() = default;
};


class Coffee : public Beverage {
public:
    std::string getDescription() const override {
        return "Coffee";
    }

    double cost() const override {
        return 5.0;  // 咖啡的基本价格
    }
};


// 装饰器类，继承自Beverage接口
class CondimentDecorator : public Beverage {
public:
    virtual std::string getDescription() const = 0;  // 获取描述
};


class Milk : public CondimentDecorator {
private:
    Beverage* beverage;  // 被装饰的对象

public:
    Milk(Beverage* bev) : beverage(bev) {}

    std::string getDescription() const override {
        return beverage->getDescription() + ", Milk";  // 在原有描述上添加牛奶
    }

    double cost() const override {
        return beverage->cost() + 1.0;  // 牛奶的附加费用
    }
};

class Sugar : public CondimentDecorator {
private:
    Beverage* beverage;

public:
    Sugar(Beverage* bev) : beverage(bev) {}

    std::string getDescription() const override {
        return beverage->getDescription() + ", Sugar";  // 在原有描述上添加糖
    }

    double cost() const override {
        return beverage->cost() + 0.5;  // 糖的附加费用
    }
};


int main() {
    // 创建一个咖啡对象
    Beverage* beverage = new Coffee();

    // 使用装饰器为咖啡添加牛奶和糖
    beverage = new Milk(beverage);
    beverage = new Sugar(beverage);

    // 输出咖啡的描述和价格
    std::cout << beverage->getDescription() << " costs " << beverage->cost() << std::endl;

    // 释放资源
    delete beverage;
    return 0;
}
