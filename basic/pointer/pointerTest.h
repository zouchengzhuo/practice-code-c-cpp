#ifndef POINTERTEST_H
#define POINTERTEST_H
#include <iostream>

class PointerTester{
    public:
        void TestInit();
        void TestConstPointer();
        //测试悬垂指针
        void TestDanglingPointer();
        //测试野指针
        void TestWildPointer();
        //智能指针
        void TestAutoPrt();
        void TestCast();
};

//一个int类型的智能指针类
class IntPtr;
//智能指针 计数类，成员都是private的，不给别人使用
class U_Ptr{
    friend class IntPtr; //设置友元类，让其可以访问private成员
    int *p;
    size_t use;
    U_Ptr(int *p):p(p), use(1){};
    ~U_Ptr(){
        delete p;
    };
};

class IntPtr{
    public:
        IntPtr(){
            ptr = nullptr;
        };
        IntPtr(int *p, const std::string &name): ptr(new U_Ptr(p)), name(name){};
        //定义复制构造函数，增加计数
        IntPtr(const IntPtr &obj):ptr(obj.ptr), name(obj.name){
            ++ptr->use;
            std::cout << name << " add by copy, current:" << ptr->use << std::endl; 
        };
        //定义赋值构造函数
        IntPtr& operator=(const IntPtr &obj){
            //右操作数的增加计数
            if(obj.ptr != nullptr){
                ++obj.ptr->use;
                std::cout << obj.name << " add by assignment, current:" << obj.ptr->use << std::endl; 
            }
            if(ptr != nullptr){
                //左操作数指针计数减1，若为0，则释放空间
                --ptr->use;
                std::cout << name << " sub by assignment, current:" << ptr->use << std::endl; 
                //如果左操作数计数已经为0，则析构之
                if(ptr->use == 0){
                    std::cout << name << " destruct p in assignment" << std::endl;
                    delete ptr;
                }
            }
            ptr = obj.ptr;
            name = obj.name;
            return *this;
        };
        ~IntPtr(){
            if(ptr == nullptr) return;
            --ptr->use;
            std::cout << name << " sub by destructor, current:" << ptr->use << std::endl; 
            if(ptr->use == 0){
                std::cout << name << " destruct p in destructor" << std::endl;
            }
        };
        int get_val(){
            return *ptr->p;
        };
        void set_val(const int *val){
            *ptr->p = *val;
        };
    private:
        U_Ptr *ptr;
        std::string name;
};

#endif