#ifndef abstract_class_h
#define abstract_class
#include <iostream>

//含有一个纯虚函数的类，就是一个抽象类
//区别于java、ts等语言，它们都有abstract关键字
class AbstractBase{
    public:
        //纯虚函数的写法
        virtual void print() = 0;
};

class AbstractSubBase: public AbstractBase{
    public:
        //纯虚函数的写法
        virtual void print() = 0;
};

class AbstractChild: public AbstractSubBase{
     public:
        //纯虚函数的写法
        virtual void print(){
            std::cout << "i am not a abstract class " << std::endl;
        };
};

void testAbstractClass(){
    //AbstractBase base; //报错： 抽象类不能初始化对象
    //AbstractSubBase subbase; //一样，还是抽象类

    AbstractChild child; //ok

    child.print();
};

#endif