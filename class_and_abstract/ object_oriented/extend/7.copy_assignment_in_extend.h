#ifndef copy_assignment_in_extend_h
#define copy_assignment_in_extend_h
#include "classextend.h"

class TestCopy: public BookBase{
    public:
        int a;
        int b;
        TestCopy(){};
        // 自己定义派生类的复制构造函数，需要手动初始化基类，再手动初始化自己的成员（如果不自己定义，编译器合成的派生类复制构造函数，也是做了这些事情的）
        TestCopy(const TestCopy& copy):BookBase(copy),a(copy.a),b(copy.b){
            std::cout << "test copy" << std::endl;
        };
        //与复制构造函数、赋值操作符不同，派生类的析构函数析构自己的成员就好了，编译器会自动依次调用派生类的析构函数
        ~TestCopy(){};
};

class TestAssignment: public BookBase{
    public:
        TestAssignment(){};
        //如果要自己定义派生类的赋值操作符，也得自己手动调用基类的赋值操作。（编译器合成的版本会自动做）
        TestAssignment& operator=(const TestAssignment& assign){
            //TODO  c++ primer中判断需要判断不能自身赋值，为什么？
            if(this != &assign){
                BookBase::operator=(assign);
            }
            std::cout << "test assignment" << std::endl;
            return *this;
        };
};

void testCopyAndAssignment(){
    //测试自定义复制构造函数
    TestCopy t1;
    TestCopy t2 = t1;
    //测试自定义赋值操作符
    TestAssignment t3, t4;
    t3 = t4;
};

#endif