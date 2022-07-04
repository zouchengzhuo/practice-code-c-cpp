#ifndef specialization_template_h
#define specialization_template_h
#include <iostream>
#include <string.h>

namespace spfn{
    //函数模板特化
    template<typename T>
    int compare(const T &t1, const T &t2){
        std::cout << "call normal template fn const char*" << std::endl;
        if(t1 < t2) return -1;
        if(t2 < t1) return 1;
        return 0;
    };
    //特化写法，空模板形参列表
    template<>
    int compare<const char*>(const char * const &t1, const char * const &t2){
        std::cout << "call sp template fn const char*" << std::endl;
        return strcmp(t1, t2);
    };

    //如果不写 template<> ...<const char*>，则是一种函数重载，调用时优先调用重载，再去调用特化模板函数
    // int compare(const char * const &t1, const char * const &t2){
    //     std::cout << "call overload fn" << std::endl;
    //     return strcmp(t1, t2);
    // };

    void testFnSpTemp(){
        //调用普通模板
        int c1 = compare(1, 2);
        const char *s1 = "abc";
        const char *s2 = "def";
        //调用特化模板
        int c2 = compare(s1, s2);
    }

};

namespace spclass{
    //普通模板累
    template<typename T>
    class Print{
        public:
            int compare(const T &t1, const T &t2){
                std::cout << "call normal template class member" << std::endl;
                if(t1 < t2) return -1;
                if(t2 < t1) return 1;
                return 0;
            };  
    };
    //特化模板类
    template<>
    class Print<const char*>{
        public:
            int compare(const char* &t1, const char* &t2){
                std::cout << "call sp template class member" << std::endl;
                return strcmp(t1, t2);
            }; 
            int compare2(const char* &t1, const char* &t2);  
    };
    //在类外定义特化模板类成员函数，不需要加 template<>标记
    int Print<const char*>::compare2(const char* &t1, const char* &t2){
        std::cout << "call sp template class member" << std::endl;
        return strcmp(t1, t2);
    }; 

    template<typename T, typename K> class Part{};
    //对于有多个模板形参的类，可以特化部分模板类型
    template<typename T> class Part<T, int>{};

    void testSpTmpClass(){
        Print<int> p1;
        int c1 = p1.compare(1, 2);
        Print<const char*> p2;
        const char *s1 = "abc";
        const char *s2 = "def";
        //调用特化模板
        int c2 = p2.compare(s1, s2);
    };
};


#endif