#include <iostream>
#include <sstream>

void testStringStream(){
    std::stringstream ss("abc");
    ss.str("def"); //abc替换为def
    ss << "ghi"; //def替换为ghi
    ss << "123"; //append 123
    std::string str = ss.str(); //通过str函数输出，不改变stream内容
    std::cout << "str in ss:" << str <<std::endl;
    std::string str2 = ss.str(); //再次输出依然有效
    std::cout << "str2 in ss:" << str2 <<std::endl;

    std::string str3, str4;
    ss >> str3; // >> 方式获取，会改变stream内容
    ss >> str4; // >> 方式再读就没了
    std::cout << "str3/4 in ss:" << str3 << " str4:" << str4 <<std::endl;

    //用read函数读取
    char strptr[3], strptr2[5];;
    std::stringstream ss2("hello world");
    ss2.read(strptr, 3); //通过read的方式，会改变内容
    ss2.read(strptr2, 5); //再次read，位置变化了
    std::string str5(strptr, strptr+3), str6(strptr2, strptr2+5);
    std::cout << "get by read:" << str5 << " str6:" << str6 <<std::endl;


};

int main(){
    testStringStream();
    return 0;
};