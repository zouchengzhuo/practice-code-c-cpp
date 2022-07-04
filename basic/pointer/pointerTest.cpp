#include "pointerTest.h"
//指针定义、初始化、赋值， 指针与引用的比较与配合
void PointerTester::TestInit(){
    std::cout << "================= TestInit start =================" << std::endl;
    //推荐 int *p1 这样的写法， int* p1这样的写法看上去有点像int*是一个数据类型
    int i1 = 10, i4 = 99;
    int *p1 = &i1;

    // '&' 与 '*'，在左值与右值中的不同含义
    int &i2 = i1;  //&在左值中，表示创建另一个变量的引用（即别名）
    int *p2 = &i2; //&在右值中，表示取地址，这里由于i2是i1的别名，&i1和&i2实际上是等价的 
                   //*在左值中，表示定义一个变量的指针
    int i3 = *p2;  //*在右值中，表示取值，获取指针指向地址对应指针数据类型的值
    //int i4 = *(p2+20000); //*在右值中可以读取任意地址的值，但是这是一个危险行为，
                            //例如这里，随便加一个很大的值，超出了进程的内存区域，会core掉，而int i4 = *(p2+4)则不会，因为还在当前进程分配的内存内
    *p1 = 8;
    int *p3 = p1;
    *p3 = 3; // p1、p2、p3指向同一个内存地址，操作的是同一个变量的值
    p3 = &i4; //可以修改指针的指向
    *p3 += 1;

    //可以有指向指针的指针， 这里意味着，指针变量本身也是一个占用内存块的对象
    int **p4 = &p3;

    std::cout << "i1:" << i1 << std::endl;
    std::cout << "i3:" << i3 << std::endl;
    std::cout << "i4:" << i4 << std::endl;
    std::cout << "p4:" << p4 << " *p4:" << *p4 << " **p4:" << **p4 << std::endl;
    std::cout << "================= TestInit end =================" << std::endl;
}

void PointerTester::TestConstPointer(){
    std::cout << "================= TestConstPointer start =================" << std::endl;
    int i1 = 1;
    int i2 = 2;
    int *const p1 = &i1;
    //p1 = &i2;  //报错： const指针的值不能修改， 不能指向另一个对象
    *p1 = 11;    //成功， const指针指向地址的值可以修改
    const int *p2 = &i2;
    //*p2 = 22;  //报错：指向const变量的指针，其指向的值不能修改
    p2 = &i1;    //成功，指针本身的指向可以修改
    p2 = &i2;
    std::cout << " *p1:" << *p1 << " *p2:" << *p2 << std::endl;

    //const与typedef配合使用时的注意点
    typedef std::string *string_ptr;
    typedef const std::string *const_string_ptr;
    std::string str1 = "abc", str2="def";
    string_ptr sp1 = &str1; //普通的字符串指针
    const string_ptr sp2 = &str1; //const指针，注意，这里的const是修饰指针的，指针指向的地址不能改变
    sp1 = &str2; //成功
    *sp2 = "xxx"; //成功
    //sp2 = &str2; //失败，const指针
    const_string_ptr sp3 = &str1; //指向const变量的指针，其指向的内存地址的值不能修改，但是本身的指向可以修改
    sp3 = &str2; //成功
    //*sp3 = "ihj"; //失败，指向const变量地址，不能修改变量值
    std::cout << " *sp1:" << *sp1 << " *sp2:" << *sp2 << " *sp3:" << *sp3<< std::endl;
    std::cout << "================= TestConstPointer end =================" << std::endl;
};

//https://www.cnblogs.com/submarinex/archive/2013/03/02/2940169.html 悬垂指针和野指针
void PointerTester::TestDanglingPointer(){
    //测试悬垂指针场景
    std::string *p = new std::string("abcdefghijk");
    std::string *p2 = p;
    std::cout << "p:" << p << " *p" << *p << std::endl;
    delete p; //释放动态分配的内存
    //std::cout << "p:" << p << " *p" << *p << std::endl; //会core掉，因为内存已经被释放，不能访问*p了
    std::cout << "p:" << p << std::endl; //可以成功，指针本身还在
    //std::cout << "p2:" << p2 << " *p2" << *p2 << std::endl; //会core掉，两者指向的是同一个地址，*p2也不能访问了

    //那么，什么样的场景会导致内存被写坏？ int好像就可以
    int *ip = new int;
    *ip = 8;
    std::cout << "ip:" << ip << " *ip:" << *ip << std::endl;
    delete ip;
    //ip = nullptr; //实践中最好加上这个
    std::cout << "ip:" << ip << " *ip:" << *ip << std::endl;
    //!!! 这种行为会导致不可预估的错误
    *ip = 999;
    std::cout << "ip:" << ip << " *ip:" << *ip << std::endl;

    //结论：悬垂指针会造成动态分配内存被写坏的问题，但不是所有数据类型都会有问题
    //至少在g++ 4.85中，标准库string类型delete了就无法操作了，而内置int类型还可以操作
};

void PointerTester::TestWildPointer(){
    std::string *p;
    //std::cout << "p:" << p << " *p" << *p << std::endl; //报错
    std::cout << "p:" << p  << std::endl; //地址是0
    int *ip;
    std::cout << "ip:" << ip  << std::endl; //地址也是0
    //结论： 所以这里不会有野指针未初始化瞎指的问题，也许是语言版本和编译器的差异
};

void PointerTester::TestCast(){
    void *p2 = new std::string("abcd");
    int *p3 = (int*)p2;
    int *p4 = static_cast<int*>(p2);
    std::cout << "&p3:" << &p3 << " p3:" << p3 << " *p3:" << *p3 << std::endl;
    std::cout << "&p4:" << &p4 << " p4:" << p4 << " *p4:" << *p4 << std::endl;
};

void PointerTester::TestAutoPrt(){
    IntPtr intptr(new int(42), "age");
    IntPtr intptr2 = intptr; //复制构造函数
    IntPtr intptr3;
    intptr3 = intptr; //赋值操作符
    IntPtr intptr4(new int(999), "price");
    intptr4 = intptr3; //赋值操作符，右操作数计数++，左操作数计数--，并到0，析构
}

int main(){
    PointerTester tester;
    //tester.TestInit();
    //tester.TestConstPointer();
    //tester.TestDanglingPointer();
    //tester.TestWildPointer();
    //tester.TestCast();
    tester.TestAutoPrt();
    return 0;
}