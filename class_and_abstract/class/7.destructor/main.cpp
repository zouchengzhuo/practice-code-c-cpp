#include <iostream>
#include <vector>

//析构函数： 1. 对象超出作用域被回收时调用 2.动态分配的内存，delete时调用
class Test{
    public:
        //自定义析构函数，执行了自定义的，编译器合成析构函数依然会被执行
        ~Test(){
            std::cout << " call destructor " << std::endl;
        };
};

void testDestructor(){
    Test *arr = new Test[3]();
    delete [] arr; //手动删除，输出三次
    arr= nullptr;
    std::vector<Test> vec(3, Test()); //会输出4次析构函数log，Test()本身一次，复制到vector容器中的三个三次
};

int main(){
    testDestructor();
};