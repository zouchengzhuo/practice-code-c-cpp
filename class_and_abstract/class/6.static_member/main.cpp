#include <iostream>

class Test{
    public:
        Test(){};
        Test& call(){
            _classCallNum += 1;
            return *this;
        };
        static int getClassCallNum(){
            return _classCallNum;
        };
    private:
        static int _classCallNum;
};
//!!! 注意 !!! static的成员，和对象没关系，没法动态分配内存，所以在编译的时候就要初始化。 这里不初始化，用起来这个成员就不存在，运行时会报错。
int Test::_classCallNum = 0;

int main(){
    Test test;
    test.call().call().call();
    //通过对象访问
    std::cout << test.getClassCallNum() << std::endl;
    //通过类名访问
    std::cout << Test::getClassCallNum() << std::endl;
}