cpp practice  

标准库列表及中文文档  
https://zh.cppreference.com/w/cpp/header  

## basic目录
一些很小的基础点的记录
- c数组相关
- c字符串相关
- 指针相关
- 引用相关
- 动态库的编译、链接
- 静态库的编译、链接  

## std_container目录
常用c++标准库模块的记录  
- std::string 字符串
- std::vector 数组容器
- std::bitset 二进制容器  
- std::map map容器
- 5.template_alg 容器模板算法   

## funtion目录
c++函数的一些特性  

- 函数指针 fnpointer
- 行内函数 inline
  
## std_io目录
io相关的一些  
- 文件io （含io迭代器等内容）
- 字符串io stringstream

## class_and_abstract目录
c++类相关  
- 1.基础定义，mutable，const成员也可以修改
- 2.隐式转换 implicit convert
- 3.firend 友元类/成员函数/普通函数
- 4.复制构造函数 copy_constructor。调用复制构造函数：Test t2 = t1
- 5.赋值操作符 assignment_operate。调用赋值操作符：Test t1, t2; t2 = t1
- 6.类的静态成员
- 7.析构函数 始终会调用编译器自己生成的
- 8.操作符重载、函数绑定器、类显式转换等（TODO 实现智能指针）  

面向对象相关
- 1.派生类基础用法，动态绑定（仅虚函数可以）
- 2.派生类强行调用基类的虚函数成员
- 3.private继承时，修改个别成员级别
- 4.友元关系不能继承， 基类的friend只能访问基类
- 5.基类与派生类的转换，static_cast和dynamic_cast的区别
- 6.派生类构造函数
- 7.派生类的赋值构造函数、赋值操作符
- 8.派生类析构函数
- 9.派生类名字查找（仅虚函数可动态绑定，派生类同名函数会覆盖基类的，无法重载）
- 10.抽象类，纯虚函数
- 11.容器和继承
- 12.句柄类和继承（动态绑定只能用指针和引用，可以用句柄类规避）

## template目录  
模板函数、模板类相关
- 1.函数模板
- 2.类模板、类内部定义泛型类相关的类型
- 3.非类型模板（常量，如数组长度等）
- 4.模板和友元
- 5.特化模板

## thread目录
c++异步相关

## linux_io目录
linux io相关

## linux_event目录
异步事件库相关