// Strategy 接口：收费策略
class PricingStrategy {
public:
    virtual double calculatePrice(double originalPrice) const = 0;
    virtual ~PricingStrategy() = default;
};


// 原价
class NormalPricing : public PricingStrategy {
public:
    double calculatePrice(double originalPrice) const override {
        return originalPrice;
    }
};

// 打8折
class DiscountPricing : public PricingStrategy {
public:
    double calculatePrice(double originalPrice) const override {
        return originalPrice * 0.8;
    }
};

// 满100减20
class CashbackPricing : public PricingStrategy {
public:
    double calculatePrice(double originalPrice) const override {
        if (originalPrice >= 100) {
            return originalPrice - 20;
        }
        return originalPrice;
    }
};


// Context 类
class CashContext {
private:
    PricingStrategy* strategy;

public:
    CashContext(PricingStrategy* s) : strategy(s) {}

    double getResult(double price) const {
        return strategy->calculatePrice(price);
    }
};


#include <iostream>

int main() {
    double totalPrice = 150.0;

    // 使用不同的策略
    CashContext context1(new NormalPricing());
    CashContext context2(new DiscountPricing());
    CashContext context3(new CashbackPricing());

    std::cout << "原价：" << context1.getResult(totalPrice) << std::endl;
    std::cout << "打八折：" << context2.getResult(totalPrice) << std::endl;
    std::cout << "满100减20：" << context3.getResult(totalPrice) << std::endl;

    return 0;
}


/* 
个人理解： 
1、一个策略基类 定义一组策略的公共方法。 
2、一组策略继承这个基类。
3、一个上下文类中保存一个策略基类。 将来把不同的子类赋值进去。
*/