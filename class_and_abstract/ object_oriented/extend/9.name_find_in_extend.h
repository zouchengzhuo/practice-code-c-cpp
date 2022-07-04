#ifndef name_find_in_extend_h
#define name_find_in_extend_h
#include <iostream>

class NamingBase{
    public:
        void print(){
            std::cout << " i am name base " << std::endl;
        };

        virtual void print2(){
            std::cout << " i am virtual in base " << std::endl;
        };
};

class NameingChild: public NamingBase{
    public:
        void print(const std::string &str){
            std::cout << " i am name child " << str << std::endl;
        };
        virtual void print2(){
            std::cout << " i am virtual in child " << std::endl;
        };
};

//派生类会覆盖同名的基类成员函数，即使参数不同，也不能重载 ( 无论是不是虚函数，都会覆盖 )
void testNameFind(){
    NameingChild nc;
    //nc.print(); //报错：
    nc.NamingBase::print();// 只能通过这样来强制调用基类的成员函数
    nc.print("test"); //调用派生类的成员函数
    NamingBase *p1 = &nc;
    p1->print();  //通过基类指针指向派生类对象，调用普通成员函数，会调用基类的成员
    p1->print2(); //通过基类指针指向派生类对象，调用虚函数成员函数，会调用派生类的成员（动态绑定就靠这东西实现了）
};

#endif