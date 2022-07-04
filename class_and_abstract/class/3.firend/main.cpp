#include <iostream>

//类和其友元类之间有相互依赖的关系, 要注意，友元类定义前，必须先定义其依赖的类
//这里的依赖关系比较复杂，总结起来：
//1. 友元类（Window）的成员函数声明依赖Screen的声明，所以声明友元前必须先声明Screen
//2. 友元类的成员函数定义依赖Screen私有成员变量的定义，所以定义友元前必须先定义Screen
//3. 类（Screen）的定义，依赖友元类的声明，所以定义类前必须声明友元类
//结论：类和其友元类的声明&定义顺序必须是： 1. 声明类 2.声明友元类 3. 定义类 4.定义友元类

class Screen;
class Window{
    public:
        void printWindowSize(const Screen &screen);
};
class Window2{
    public:
        void printWindowSize(const Screen &screen);
};

class Screen{
    //定义友元类，可以访问私有成员
    friend class Window;
    //将某个类的某个成员定义为友元函数
    friend void Window2::printWindowSize(const Screen &screen);
    //将某个函数设置为友元函数
    friend void printWindowSize(const Screen &screen);
    public:
        Screen(){};
        Screen(const int &width, const int &height):_width(width), _height(height){};
    private:
        int _width;
        int _height;
};
void Window::printWindowSize(const Screen &screen){
    std::cout << "width:" << std::to_string(screen._width) << " height:" << std::to_string(screen._height) << std::endl;
};

void Window2::printWindowSize(const Screen &screen){
    std::cout << "width:" << std::to_string(screen._width) << " height:" << std::to_string(screen._height) << std::endl;
};

void printWindowSize(const Screen &screen){
    std::cout << "width:" << std::to_string(screen._width) << " height:" << std::to_string(screen._height) << std::endl;
};

int main(){
    Window window;
    Window2 window2;
    Screen screen(960, 1600);
    window.printWindowSize(screen);
    window2.printWindowSize(screen);
    printWindowSize(screen);
}