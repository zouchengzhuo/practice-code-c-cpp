#include <fstream>
#include <iostream>

void testIfstream(){
    std::ifstream in;
    in.open("testfile.txt");
    if(!in){
        std::cout << "error: open file fail" << std::endl;
        return;
    }
    std::string s;
    while (in >> s)
    {
        std::cout << "read from file:" << s << std::endl;
    }
};

int testClearStatus(){
    std::string files[] = {"testfile.txt", "testfile2.txt"};
    std::string s;
    //第一种搞法，每次创建新的ifstream
    for(int i=0;i<2;i++){
        std::ifstream in(*(files+i));
        while (in >> s)
        {
            std::cout << s << std::endl;
        };
    }
    //第二种搞法， 服用ifstream对象，但是就得clear状态了
    std::ifstream globalin;
    for(int i=0;i<2;i++){
        globalin.open(*(files+i));
        while (globalin >> s)
        {
            std::cout << s << std::endl;
        };
        globalin.close();
        globalin.clear(); //TODO c++ primer上说必须要clear才能重新open，但是这里不clear也可以
    }
};

//测试打开文件类型
// in:输入 out:输出 app:尾部写入 ate:打开文件后立即将文件定位在文件末尾 trunc:打开文件时清空已存在的文件流 binary：以二进制模式进行文件io操作
// 多种模式可以用按位与来叠加
// 这里要注意的时 ofstream默认时 out | trunct，即会清空文件
int testFileType(){
    //std::ofstream out("testfile2.txt"); //这个操作会清空testfile2.txt
    //std::fstream out("testfile2.txt", std::fstream::out); //也会清空
    std::fstream out("testfile2.txt", std::fstream::out | std::fstream::app); //！！！ 不会清空，这里要注意，如果不想清空内容，一定要加上app模式
    out << "\naaaaaa";
    out << "\nbbbbbb";
    out.flush();
};

//把输入流绑定到输出流
//std的cin默认是绑定到cout的， 这个貌似只是给输入流附加一个保存在内部的输出流对象， 方便取，没其它任何含义？
void testTie(){
   //https://www.yiibai.com/cpp_standard_library/cpp_ios_tie.html
   std::cout << "cout:" << &std::cout << std::endl;
   std::cout << "cin tie:" << std::cin.tie() << std::endl;
   
   std::ostream *prevstr;
   std::ofstream ofs;
   ofs.open ("test.txt");

   std::cout << "tie example:\n";
   *std::cin.tie() << "This is inserted into cout";
   prevstr = std::cin.tie (&ofs);
   *std::cin.tie() << "This is inserted into the file";
   std::cin.tie (prevstr);
   ofs.close();
};

//测试将对象写入到文件、从文件读取对象
class Company{
    public:
        Company(){};
        Company(const int &name, const int &age, const int &address):name(name), age(age), address(address){};
        //std::string name;
        int name;
        int age;
        //std::string address;
        int address;
};

//测试简单的将对象写入文件、从文件读取对象（5.template_alg中有用流迭代器做这种操作的用法）
void testWriteReadObj(){
    //将对象的二进制写入二进制文件
    std::ofstream out;
    //一开始类有三个属性，包含两个std::string类型， 这里长度总是返回24，从文件读写会出问题，这里有变长字段时无法直接read
    //可以自己重载 >> <<运算符来处理含变长类型数据时的序列化/反序列化
    //Company obj("czzou china wuhan", 28, "china wuhan china wuhan china wuhan china wuhan");
    Company obj(999, 123456, 888);
    out.open("myclass.obj");
    
    unsigned int size = sizeof obj;
    std::cout << "obj size:" << size << std::endl;
    out.write((char*)&obj, size);
    out.close();

    //从二进制文件中读取对象
    std::ifstream in;
    Company objFromFile;
    in.open("myclass.obj", std::ifstream::binary);
    in.read((char*)&objFromFile, sizeof objFromFile);
    in.close();
    std::cout << "objFromFile.age:" << objFromFile.age << std::endl;
};

//从终端读取，写入文件
void testBasicIO(){
    std::ofstream ofs;
    ofs.open ("from_cin.txt");
    std::cin.tie(&ofs);
    std::string s;
    while (std::cin >> s)
    {
        *std::cin.tie() << s;
    }    
    
};
int main(){
    //testIfstream();
    //testClearStatus();
    //testFileType();
    //testTie();
    //testWriteReadObj();
    //testBasicIO();
    return 0;
};