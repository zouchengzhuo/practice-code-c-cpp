//标准库string
#include "stringTest.h"

void StringTester::TestInit(){
    std::cout << "================= TestInit start =================" << std::endl;
    std::string s1;
    s1 = "abc";
    std::cout << "s1:" << s1 << std::endl;
    std::string s2(s1); //copy一个副本
    std::cout << "s2:" << s2 << std::endl;
    std::string s3("def");
    std::cout << "s3:" << s3 << std::endl;
    std::string s4(10, 'a');
    std::cout << "s4:" << s4 << std::endl;
    std::cout << "================= TestInit end   =================" << std::endl;
}

int main(){
    StringTester tester;
    tester.TestInit();
    return 0;
}