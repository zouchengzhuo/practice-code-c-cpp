#include <iostream>
#include <vector>

class ClassA{
    public:
        ClassA(){};
        ClassA(const char str[]){};
        //ClassA(const ClassA &obj):a(obj.a), b(obj.b), c(obj.c){};  //默认的合成复制构造函数长这样
        //可以自定义复制构造函数，一般它是不被设置为explicit的，即不能禁止隐式类型转换
        ClassA(const ClassA &obj):a(obj.a), b(obj.b), c(obj.c){
            std::cout << "call copy" << std::endl;
        };
        ClassA fn1(ClassA a){
            return *this;
        };
    public:
        std::string a;
        std::string b;
        std::string c;
    private:
        // 禁止复制一个对象，将复制构造函数放private下面就行了，如iostream就是不允许复制的
        // ClassA(const ClassA &obj):a(obj.a), b(obj.b), c(obj.c){
        //     std::cout << "call copy" << std::endl;
        // };
};

//什么时候会调用复制构造函数，当没有手动创建复制构造函数时，编译器会默认创建一个
//这个默认的叫做 “合成复制构造函数 synthesized copy constructor”，会逐个初始化非static的成员
void WhenCall(){
    //1.从一个对象初始化另一个，调用copy构造函数
    ClassA a1;//直接初始化，调用普通构造函数
    ClassA a2 = a1; //copy初始化
    ClassA a_copy = ClassA();//调用复制构造函数初始化a_copy，和上面的赋值是等价的
    //2.非引用类型的函数参数，会调用ClassA的复制构造函数，由a1构造形参a
    a2.fn1(a1);
    //3.非引用类型的函数返回值
    ClassA a3 = a2.fn1(a1);
    //4.std顺序容器的初始化
    ClassA arr[] = {a1, a2, a3};
    std::vector<ClassA> vec(arr, arr+2);
    //5.初始化数组元素
    ClassA a4 = "abc"; //这样会调用默认构造函数
    ClassA arr2[] = {"def", "ghi", ClassA()}; //但是在花括号中是，会先调用构造函数，再调用复制构造函数初始化每个数组元素的值
};

int main(){
    WhenCall();
};