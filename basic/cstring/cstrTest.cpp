#include "cstrTest.h"

void CStrTester::TestInit(){
    std::cout << "================= TestInit start =================" << std::endl;
    //c++中的字符串字面量实际上就是一个c风格字符串，等价于 const char[]
    char c1[] = {'a', 'b', 'c'}; //普通的char数组
    char c2[] = {'a', 'b', 'c', '\0'}; //末尾加了个 NULL， 是c风格字符串
    char c3[] = "abc";  //这种字符串字面量，会自动在末尾加上NULL，是c风格字符串
    const char *p1 = "abc";   //会在末尾添加 NULL
    const char c4[] = "abc";
    std::string str1 = p1; //等价于 str1 = "abc";
    str1 = c4;  //等价于 str1 = "abc";
    std::cout << "str1:" << str1 << std::endl;

    //从std的string初始化
    std::string str2 = "test str";
    const char *c5 = str2.c_str();
    std::cout << "c5:" << c5 << std::endl;

    std::cout << "================= TestInit end =================" << std::endl;
};

void CStrTester::TestStdCStr(){
    std::cout << "================= TestStdCStr start =================" << std::endl;
    const char c1[] = {'a', 'b', 'c', '\0'};
    const char *c2 = "abc";
    const char *c3 = "def";

    //strlen函数，获取cstring的长度，一般的cpp数组是无法获取长度的，但是这个cstr可以根据末尾的null来判断
    std::cout << "strlen(c1):" << std::strlen(c1) << std::endl;

    //strcmp cstr比较函数, 有点奇葩： 相等返回0， c1>c2，返回正数，c1<c2，返回负数
    std::cout << "strcmp(c1,c2):" << std::strcmp(c1,c2) << std::endl;

    //strcat(s1, s2) 字符串连接，将字符串s2连接到s1，并返回s1,这里返回的是一个指针变量， 与s1指向同一个地址，但是并不是s1指针的引用
    //const char *c4 = std::strcat(c1, c2); //错误，c1是const的，不能修改
    //⬇错误，会运行时core掉，因为c4实际上是一个 “指向const对象的指针”， 被指向的对象是不能修改的，改成 char c4[] = "ghi"，走一遍初始化，就行了
    //char *c4 = "ghi"; 
    char c4[2] = {'a','\0'}; //正确， 这里长度只有2，按c++ primer里边的说法，实践时一定要保证长度足够
    //一个有趣的现象：目前这样返回的是 aabc, 而去掉末尾的'\0'，返回的结果是 aabcabc， 看起来是先拼接出来一个\0，再在\0后拼接一次
    //char c4[2] = {'a'}，这样指定长度，然后去掉末尾的\0，也能得到正确的 aabc
    char * const c4_const = c4; //正确，指针是const的无所谓，指向的变量不是const的就行
    char *c5 = std::strcat(c4_const, c1);
    std::cout << "strcat(c4, c1), c4:" << c4 << " c5:" << c5 << " &c4:" << &c4 << " &c5:" << &c5 << std::endl;

    //strcpy(s1, s2),将s2的内容copy给s1，覆盖s1的内容，并返回s1指针，这个也要注意长度问题，长度不够直接跪
    char c6[] = "abc"; //注意：改成 char *c6 = "abc"，则不行
    char *c7 = std::strcpy(c6,"aaaaaaqqqqdddddd");
    std::cout << "strcpy(s1, s2) c6:" << c6 << " c7:" << c7  << std::endl;

    //strcat(s1, s2, n)、strcpy(s1, s2, n)， 区别是只操作s2的前n个字符，略过
    std::cout << "================= TestStdCStr end =================" << std::endl;
};

void CStrTester::TestWithStdString(){

};

int main(){
    CStrTester tester;
    tester.TestInit();
    //tester.TestStdCStr();
    return 0;
}