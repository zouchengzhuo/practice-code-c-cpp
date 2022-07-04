#ifndef basic_extend_h
#define basic_extend_h
#include "classextend.h"
// 继承级别：
//public 基类的成员在派生类中保存原访问级别 称为“接口继承”
//protected 基类的public变为protected 称为”实现继承“
//private 基类的所有成员在派生类中都是private 称为”实现继承“
class RedBook : public BookBase {
    public:
       RedBook(const std::string &name, const int &price, const std::string &serires): BookBase(name, price), serires(serires){};
       void printName() const{
           std::cout << "book name in RedBook: " << this->serires << "-" << this->name << std::endl;
       }
    private:
        std::string serires;
};


//protected或者private继承，可以改变个别成员的访问级别，但是不能超过基类中的级别
class MyBook : private BookBase{
    public:
        using BookBase::BookBase;
        using BookBase::printName;
};
//1.测试使用派生类
void printBookInfo(const BookBase &book){
    book.printName();
};
//2.测试派生类强行调用基类的虚函数
void printBookInfoInBase(const RedBook &book){
    const BookBase *bookptr = &book;
    bookptr->BookBase::printName();
    book.BookBase::printName();
};
//3.测试private继承修改个别成员级别
void testChangeOne(){
    const MyBook mybook("mybook",88);
    mybook.printName();
};

//4. 友元关系不能继承， 基类的friend只能访问基类

#endif