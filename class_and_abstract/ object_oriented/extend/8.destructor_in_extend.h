#ifndef destructor_in_extend
#define destructor_in_extend
#include <iostream>

//一般实现了自定义析构函数，往往意味着需要实现其它自定义复制控制成员，如复制构造函数、赋值操作符
//但是基类为了把构造函数设置称虚函数时，除外
class Base1{
    public:
        ~Base1(){
            std::cout << " call ~Base1 " << std::endl;
        };
};

// 这里原理是因为，一个基类的指针指向派生类成员，名字查找时，对于非虚函数成员，会直接找到基类的，但是对于虚函数成员，会去调用派生类的实现
// 这个名字查找的原理也是面向对象动态绑定实现的基础
// 基于此，基类一定要提供虚析构函数，这样在动态绑定的派生类对象析构时，才会去调用派生类的析构函数
class Base2{
    public:
        virtual ~Base2(){
            std::cout << " call ~Base2 " << std::endl;
        };
};

class Children1:public Base1{
    public:
        ~Children1(){
                std::cout << " call ~Children1 " << std::endl;
        };
};

class Children2:public Base2{
    public:
        ~Children2(){
                std::cout << " call ~Children2 " << std::endl;
        };
};

void testDestructor(){
    Base1 *p1 = new Base1;
    delete p1; //ok 析构了base1对象
    p1 = new Children1;
    delete p1; //运行ok，但是析构的还是base1对象，Children1对象中可能存在未被释放的成员

    Base2 *p2 = new Base2;
    delete p2; //ok 析构了base2对象
    p2 = new Children2;
    delete p2; //ok 调用了children2和base2的析构函数，析构顺序为 ~Children2，然后 ~Base2
};

#endif