
#include <iostream>

class CaffeineBeverage {
public:
    // 模板方法：不可更改
    void prepareRecipe() {
        boilWater();
        brew();              // 子类实现
        pourInCup();
        if (customerWantsCondiments()) {
            addCondiments(); // 子类实现
        }
    }

    virtual ~CaffeineBeverage() = default;

protected:
    void boilWater() {
        std::cout << "Boiling water\n";
    }

    void pourInCup() {
        std::cout << "Pouring into cup\n";
    }

    virtual void brew() = 0;              // 冲泡（抽象）
    virtual void addCondiments() = 0;     // 加调料（抽象）

    // 钩子方法：可选步骤
    virtual bool customerWantsCondiments() {
        return true;
    }
};


class Tea : public CaffeineBeverage {
protected:
    void brew() override {
        std::cout << "Steeping the tea\n";
    }

    void addCondiments() override {
        std::cout << "Adding lemon\n";
    }
};


class Coffee : public CaffeineBeverage {
protected:
    void brew() override {
        std::cout << "Dripping coffee through filter\n";
    }

    void addCondiments() override {
        std::cout << "Adding sugar and milk\n";
    }

    bool customerWantsCondiments() override {
        return false;  // 假设顾客不加糖奶
    }
};

int main() {
    CaffeineBeverage* tea = new Tea();
    std::cout << "Making tea:\n";
    tea->prepareRecipe();
    delete tea;

    std::cout << "\n";

    CaffeineBeverage* coffee = new Coffee();
    std::cout << "Making coffee:\n";
    coffee->prepareRecipe();
    delete coffee;

    return 0;
}

/*
个人理解：
1、基类定义模板，模板中要调用其他函数，完成一定的功能。可以实现一部分函数。
2、子类继承父类，实现其中的虚函数。
*/