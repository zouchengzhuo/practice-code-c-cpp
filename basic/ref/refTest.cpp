//引用定义与操作
#include <iostream>

int main(){
    std::string a = "abc";
    std::string &b =a;
    b = "def";
    std::cout << "a:" << a << " b:" << b << std::endl;
    return 0;
}