#include <iostream>

class ClassA{
    public:
        ClassA(){};
        ClassA(const char str[]){};
        ClassA(const ClassA &obj):a(obj.a), b(obj.b), c(obj.c){
            std::cout << "call copy" << std::endl;
        };
        ClassA& operator=(const ClassA &a){
            std::cout << "call assignment" << std::endl;
        };
    public:
        std::string a;
        std::string b;
        std::string c;
};

int main(){
    ClassA a1, a2;
    ClassA a3 = a1; //调用复制构造函数
    a2 = a1; //调用赋值操作符
}