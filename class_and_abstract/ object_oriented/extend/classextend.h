#ifndef classextend_h
#define classextend_h
#include <iostream>
class BookBase{
    public:
        BookBase(){};
        BookBase(const std::string &name, const int &price): name(name), price(price){};
        std::string name;
        int price;
        //虚函数，在派生类中可以改写
        virtual void printName() const{
            std::cout << "book name in BookBase: " << this->name << std::endl;
        };
        //8.destructor中解释了基类为啥总是需要虚析构函数
        virtual ~BookBase(){};
};


#endif