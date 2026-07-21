#pragma once
#include <iostream>
#include <memory>

template<typename T>
class   Singleton {
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton<T>& operator = (const Singleton<T>&) = delete;

public:
    static std::shared_ptr<T> Get_instance() {
        static std::shared_ptr<T> _instance(new T);
        return _instance;
    }

    void PrintAddr() {
        std::cout << "Singleton addr " << Get_instance().get() << std::endl;
    }

    ~Singleton() {
        std::cout << "Singleton destructed here. " << std::endl;
    }
};
