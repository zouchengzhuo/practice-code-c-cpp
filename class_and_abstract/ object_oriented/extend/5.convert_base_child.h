#ifndef convert_h
#define convert_h

#include "classextend.h"

//5. 派生类到基类转换，可以直接转换， 基类到派生类，必须手动static_cast / dynamic_cast
// 基本类型推荐使用static_cast， 类类型推荐使用dynamic_cast
void testConvert(){
    RedBook book("a", 1, "b");
    MyBook mybook("a", 1);
    //派生类——基类，可以直接基于派生类创建基类的引用、指针
    BookBase &bbook = book; 
    BookBase *bbookptr = &book;
    //基类——派生类，只能static cast
    //RedBook &rbook = bbook; //编译报错
    //RedBook *rbookptr = &bbook;//编译报错
    //RedBook rbook = static_cast<RedBook>(bbook);//编译报错，貌似只能处理指针
    RedBook *rbookptr = static_cast<RedBook*>(&bbook); //ok
    RedBook *rbookptr2 = dynamic_cast<RedBook*>(&bbook);//ok
    //那么区别在哪里呢？
    //RedBook *rbookptr3 = static_cast<RedBook*>(&mybook);//编译报错
    RedBook *rbookptr4 = dynamic_cast<RedBook*>(&mybook); //只是warning，在运行时做检查
    //因为类型不匹配，如果不做这个检查，运行时会core掉
    if(rbookptr4 != nullptr){
        rbookptr4->printName();
    }
    
    std::cout << "success!" << std::endl;
};
#endif