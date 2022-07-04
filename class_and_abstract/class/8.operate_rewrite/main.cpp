#include <iostream>
#include <bitset>
#include <functional>
#include <vector>
#include <algorithm>

//可以重载的操作符
//四则运算 + - * / % += -+ /= %=
//位运算 ^ & | ~ ^= &= |= ~=
//取余 % 、取反 ! 、逗号 ,、赋值 = 
//io操作 <<、>>、<<=、>>=
//判断、赋值等 >、>=、<、<=、++、--、==、!=、&&、||、
//索引/调用/访问 [] () -> ->* 
//动态内存 new new[] delete delete []
//不可以重载的操作符 :: 、 .* 、 . 、 ?:

//是否成员规则：
//赋值=、下标 []、调用 ()、成员访问 -> ，必须定义为成员，否则报错
//io 操作，必须定义为非成员
//算术、关系、位操作符，推荐定义为非成员
//自增自减、解引用，推荐定义为成员

class Test{
    public:
        //必须加友元函数，不然io操作函数无法访问private对象
        friend std::ostream& operator<<(std::ostream &out, const Test &t);
        Test(const int &i, const std::bitset<8> b):i(i), b(b){};
        //这里是不好的实践，最佳实践：
        //1.运算操作、位运算操作，可以定义为成员的， 但是更推荐定义为非成员的
        //2.与内置加法保持一致，加法不修改被操作对象，加法作用的两个对象应该都是const引用，而不是像这里返回引用。不过这样搞也不会报错，效率也理应更高。
        Test& operator+(const Test& t){
            this->i += t.i;
            return *this;
        };
        Test& operator|(const Test& t){
            this->b |= t.b;
            return *this;
        }
        //下面的会编译报错，io操作符必须定义为非成员
        // std::ostream& operator<<(std::ostream &out){}
        //赋值操作符，必须定义为类成员，否则报错
        Test& operator=(const Test& t){
            std::cout << " call assignment" << std::endl;
        };
        Test(const Test &t):i(t.i), b(t.b){
            std::cout << "call copy" << std::endl;
        };
    private:
        int i;
        std::bitset<8> b;
};

std::ostream& operator<<(std::ostream &out, const Test &t){
    out << "[" << t.i << "," << t.b << "]" << std::endl;
    return out;
}

////////////   一般操作符重载
void testRewrite(){
    Test t1(10, std::bitset<8>(0xae));
    Test t2(8, std::bitset<8>(0xff));
    std::cout << t1;
    //t1 + t2;//其实这样就可以了，与下面的区别是，下面的多调了一次赋值函数
    t1  = t1 + t2;
    std::cout << t1;
    t1 | t2;
    std::cout << t1;
    Test t3 = t1;
    std::cout << t3;
    Test *p1 = &t1;
    Test *p2 = p1;
    std::cout << &p1 << "," << &p2 << std::endl;
};

////////////////    标准库提供的操作符函数
void testStdOp(){
    int a=2, b=5;
    std::string s1="hello ", s2="world";

    std::plus<int> plusInt;
    int c = plusInt(a, b);
    std::cout << " std::plus<int>()(a, b):" << c << std::endl;
    //注意：下面这种写法会报错，首先这些运算函数实际上是泛型结构体，需要先实例化再使用
    // std::string s3 = std::plus<std::string>(s1, s2);
    // std::cout << " std::plus<int>(a, b):" << s3 << std::endl;
};

///////////////////    binder和Negator测试，用于count_if快速统计等场景
void testBinderAndNegator(){
    int a[] = {1, 2, 3, 4, 5, 6, 7 ,8 ,9};
    std::vector<int> v1(a, a+9);
    //binder用法
    //快速统计小于3的数量，用标准库的比较运算函数，绑定第二个参数，也就是右值
    int lt3 = std::count_if(v1.begin(), v1.end(), std::bind2nd(std::less<int>(), 3));
    std::cout << " less than 3:" << lt3 << std::endl;

    //not 求反用法
    //快速统计不小于3的数量，可以用greater_equal函数，或者用not
    int notlt3 = std::count_if(v1.begin(), v1.end(), std::not1( std::bind2nd(std::less<int>(), 3) ));
    std::cout << "not less than 3:" << notlt3 << std::endl;
};



class ClassA{
    public:
        ClassA(const int &v1):v1(v1){};
        int v1;

};
class ClassB{
    public:
        ClassB(const int &v1):v1(v1){};
        int v1;
        operator ClassA(){
            return *new ClassA(this->v1);
        }
};

//////////////////   类型转换操作符
void testConvert(){
    ClassB b(4);
    //赋值的时候类型转换
    ClassA a = b;
    //显式转换
    ClassA a2 = static_cast<ClassA>(b);
    ClassA a3 = (ClassA)b;
    std::cout << "a.v1:" << a.v1 << std::endl;
    std::cout << "a2.v1:" << a2.v1 << std::endl;
    std::cout << "a3.v1:" << a3.v1 << std::endl;
}


int main(){
    //testStdOp();
    //testBinderAndNegator();
    testConvert();
};