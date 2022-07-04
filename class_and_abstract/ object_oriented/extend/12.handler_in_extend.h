#ifndef handler_in_extend_h
#define handler_in_extend_h

#include <iostream>

class HandleBase{
    public:
        virtual void print(){
            std::cout << " i am base " << std::endl;
        };
};

class HandleChild: public HandleBase{
    public:
        virtual void print(){
            std::cout << " i am child " << std::endl;
        };
};

//三种方式传递参数：值传递（会调用复制构造函数）、引用传递（无开销）、指针传递（创建一个指针变量）
//如果不想传递指针或者引用， 那么一般会用句柄类，也就是封装一个handle类，来管理指针。 构造方法略过，和智能指针类似，(c++primer 15.8节)
void testFn(HandleBase obj, HandleBase &ref, HandleBase *ptr){
    obj.print(); //打出base
    ref.print(); //打出child
    ptr->print(); //打出child
};

void testHandle(){
    HandleChild child;
    testFn(child, child, &child);
};

#endif