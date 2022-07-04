#include <iostream>

class Test{
    //public定义的成员，叫数据抽象
    public:
        int a;
        //在类内部定义类别名
        typedef std::string MyString;
        MyString mystr;
    //private定义的成员，叫数据封装
    private:
        std::string b;
        //mutable成员， const函数也可以修改。 //TODO，为啥要这样设计，何时会用到呢
        mutable int c; 
    public:
        Test(){};
        Test(const int &a, const std::string &b):a(a), b(b){};
        //指定默认参数
        Test(const std::string &b = "default"): b(b){};
        //在类内部定义的成员函数，默认是inline的
        //带const的成员函数，常量成员， 不能修改此对象的属性值
        const int returnA() const{
            return this->a;
        };
        const int modifyA(){
            this->a = 888;
            return this->a;
        };
        Test& modifyChain(const int& i){
            this->a += i;
            return *this;
        };
        Test& display(){
            std::cout << "call normal" << std::endl;
            this->_display();//这个display是const成员，注意private的 display也要是const的
            return *this;
        };
        const Test& display() const{
            std::cout << "call const" << std::endl;
            this->_display();//这个display是const成员，注意private的 display也要是const的
            return *this;
        };
        const std::string returnB() const;
    private:
        void _display() const{ 
            std::cout << this->b << "," << std::to_string(this->a) << std::endl;
        };
};
//类外定义的成员函数，默认不是inline的， 但是可以手动声明为inline的
inline const std::string Test::returnB() const {
    return this->b;
}

//基础写法
void testBasic(){
    Test test(1, "testb");
    std::cout << test.a << std::endl;
    test.modifyA();
    std::cout << test.a << std::endl;
    std::cout << test.returnB() << std::endl;
};

//链式调用
void testChianCall(){
    Test test(1, "testb");
    //普通链式调用
    test.modifyChain(2).modifyChain(3);
    //display是有const重载的成员函数， 链式调用时，自动选择非const的
    test.modifyChain(4).display();
    //直接调用时，选择const的 //TODO，按c++ primer的说法，应该是这样，但是实际都会调用normal的
    test.display();
    std::cout << test.a << std::endl;
};

int main(){
    //testBasic();
    testChianCall();
};