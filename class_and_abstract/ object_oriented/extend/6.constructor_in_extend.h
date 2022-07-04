#ifndef constructor_in_extend_h
#define constructor_in_extend_h
#include "classextend.h"

class TestBaseConstructor: public BookBase{
    public:
        //可以这样访问父类的构造函数
        TestBaseConstructor(const std::string &name, const int &price, const int &a, const int &b):BookBase(name, price), a(a),b(b){};
        //指定默认值
        //TestBaseConstructor(const std::string &name = "", const int &price = 9, const int &a = 0, const int &b = 0):BookBase(name, price), a(a),b(b){};
        int a;
        int b;

        //虚函数重新实现，还是虚函数，这里手动指定或者不手动指定都一样
        void printName() const{
           std::cout << "book name in TestBaseConstructor: " << this->name << std::endl;
        }
};

class TestSubSubClass: public TestBaseConstructor{
    public:
        //报错，只能初始化自己的直接基类，不能初始化基类的基类
        // TestSubSubClass(const std::string &name, const int &price, 
        //     const int &a, const int &b, const int &c): BookBase(name, price), a(a),b(b),c(c){};
         TestSubSubClass(const std::string &name, const int &price, 
            const int &a, const int &b, const int &c): TestBaseConstructor(name, price,a,b),c(c){};
        
        int c;

        void printName() const{
           std::cout << "book name in TestSubSubClass: " << this->name << std::endl;
        }
};

void testExtendConstructor(){
    TestSubSubClass subsub("subsubbook", 1,2,3,4);
    //调用自己重写的成员
    subsub.printName();
    //调用基类重写的成员
    subsub.TestBaseConstructor::printName();
    //调用基类的基类重写的成员
    subsub.TestBaseConstructor::BookBase::printName();
};

#endif