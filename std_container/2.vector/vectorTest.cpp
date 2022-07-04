#include "vectorTest.h"

void VectorTester::TestInit(){
    std::cout << "================= TestInit start =================" << std::endl;
    std::vector<int> v1;
    v1.push_back(1);
    v1.push_back(2);
    std::vector<int> v2(v1);//复制v1的副本
    std::vector<int> v3(10,5);//初始化为10个5
    std::vector<int> v4(10);//10个0

    //从c风格数组初始化std vector，传入两头的指针
    char c1[] = "abcde";
    std::vector<char> v5(c1, c1+4);
    std::cout << "v1 size:" << v1.size() << " [0]:" << v1[0] << std::endl;
    std::cout << "v2 size:" << v2.size() << " [0]:" << v2[0] << std::endl;
    std::cout << "v3 size:" << v3.size() << " [0]:" << v3[0] << std::endl;
    std::cout << "v4 size:" << v4.size() << " [0]:" << v4[0] << std::endl;
    std::cout << "v5 size:" << v5.size() << " [0]:" << v5[0] << std::endl;
    std::cout << "================= TestInit end   =================" << std::endl;
}

void VectorTester::TestIter(){
    std::cout << "================= TestIter start =================" << std::endl;
    std::vector<std::string> v1;
    v1.push_back("abc");
    v1.push_back("def");
    v1.push_back("hij");
    for(std::vector<std::string>::iterator iter = v1.begin(); iter != v1.end(); ++iter){
        //iter打出来的地址是一样的，value是不一样的,  &iter指向的是这个迭代器变量的内存地址，这个内存地址一直没有变化，只是其值一直在变化
        std::cout << "iter:" << &iter << " value:" << *iter << std::endl;
    }
    std::string *p = &v1[0];
    for(int i=0;i<v1.size();i++){
        //指针地址和value的值同时变化
        std::cout << "p:" << p << " value:" << *p << std::endl;
        p++;
    }
    std::cout << "================= TestIter start =================" << std::endl;
}

int main(){
    VectorTester tester;
    //tester.TestInit();
    tester.TestIter();
    return 0;
};