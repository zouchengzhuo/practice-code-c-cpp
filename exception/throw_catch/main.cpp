#include <iostream>
#include <stdexcept>
void testThrowString(){
    std::string err = "throw string";
    throw err;
};

void testThrowRuntimeError(){
    throw std::runtime_error("throw runtime error");
};

void testThrowArgumentError(){
    throw std::invalid_argument("throw invalid_argument error");
};


int main(){
    try{
        testThrowString();
    } catch(std::string &e){
        std::cout << "e:" << e << std::endl;
    }

    //多重catch，catch对象如果有继承关系，要从低向高层次写，不然低级派生对象永远catch不到了
    //查找与抛出类型匹配的catch，过程称之为：栈展开
    //栈展开期间，会释放局部变量，并调用局部对象的析构函数
    try{
        testThrowRuntimeError();
    } catch(std::string &e){
        std::cout << "e:" << e << std::endl;
    } catch(std::runtime_error &e){
        std::cout << "e:" << e.what() << std::endl;
    } catch(std::exception &e){
        std::cout << "e:" << e.what() << std::endl;
    }

    //捕获所有异常，这个catch一定是要放到最后的
    try{
        testThrowRuntimeError();
    } catch(...){
        std::cout << "unknow error"  <<std::endl;
    }
}