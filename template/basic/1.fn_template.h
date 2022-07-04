#ifndef fb_template_h
#define fb_template_h
#include <iostream>
namespace fn{

    //基本的函数模板
    template <typename T>
    int compare(const T &v1, const T &v2){
        std::cout << " compare1 " << std::endl;
        //这不是好的实践， 原则是对模板类型要求尽量低， 所以用两个小于来比较是最好的
        if(v1 > v2) return -1;
        if(v1 < v2) return 1;
        return 0;
    };

    //模板函数重载，多个模板形参，逗号分割
    template<typename T,typename K>
    int compare(const T &v1, const K &v2){
        std::cout << " compare2 " << std::endl;
        if(v1 > v2) return -1;
        if(v1 < v2) return 1;
        return 0;
    };
    
    //调用时，会自动推断模版类型，选择合适的函数调用
    void testFnTemplate(){
        int c1 = compare(1, 2);
        int c2 = compare(3, 3.45f);
        //也可以手动指定模板参数列表
        int c3 = compare<int, float>(1, 2);
        std::cout << "c1:" << c1 << " c2:" << c2 << " c3:" << c3 << std::endl;
    };
};

#endif