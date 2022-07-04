#include <iostream>

class Book{
    public:
        std::string name;
        int price;
    public:
        Book(){};
        //像这样只有一个参数的构造函数，可以用于隐士类型转换
        Book(const std::string &name):name(name){};
        Book(const char name[]):name(name){};
        //禁止隐式类型转换
        explicit Book(const int price):price(price){};
};

void LogBookName(const Book &book){
    std::cout << book.name << std::endl;
};

void LogBookPrice(const Book &book){
    std::cout << book.price << std::endl;
};

int main(){
    //测试隐式类型转换
    std::string name = "implicit book";
    LogBookName(name);
    LogBookName("implicit book by char[]");
    //LogBookPrice(5);//不行，因为price单参数构造函数是定义成explicit的
    LogBookPrice(Book(5));
}