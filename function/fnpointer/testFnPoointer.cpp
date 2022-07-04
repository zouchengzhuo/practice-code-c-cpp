#include <iostream>

bool compare1(const int &a, const int &b){
    return a > b;
}
bool compare2(const int &a, const int &b){
    return a < b;
}

///////////////////////////////1. 通过函数指针调用函数
void testUseFnPointer(){
    //函数指针， 理解方法也简单，把函数名用 (*xxx) 替代就行了
    bool (*cmp)(const int &x, const int &y);
    cmp = &compare1;
    bool res1 = cmp(3,4);
    cmp = &compare2;
    bool res2 = cmp(3,4);
    std::cout << "res1:"<<res1<<" res2:"<<res2 <<std::endl;
    
    //typedef函数指针
    typedef bool (*cmp_fn_ptr)(const int &x, const int &y);
    cmp_fn_ptr fnptr = &compare1;
    bool res3 = fnptr(3,4);
    fnptr = &compare2;
    bool res4 = fnptr(3,4);
    std::cout << "res3:"<<res3<<" res4:"<<res4 <<std::endl;


};

///////////////// 2. 指向类成员函数的指针
class Test{
    public:
        int add(const int &a, const int &b);
};

int Test::add(const int &a, const int &b){
    return a+b;
};

void pointerToMemberFn(){
    //指向成员函数的指针
    //很奇怪的写法，赋值时用类而不是类实例：&Test::add  调用时用类指针的另一个指针属性，不常用的话难记哦 (testp->*addfn)(99,88) 
    Test test;
    Test *testp = &test;
    int (Test::*addfn) (const int &x, const int &y) = &Test::add;
    std::cout << "pointer of member fn add:"<< (testp->*addfn)(99,88) <<std::endl;
};

//////////////////// 3. 函数指针作为形参

bool selectCmp(bool (*cmp)(const int &x, const int &y), const int &x, const int &y){
    return cmp(x, y);
};

void testPointerAsParams(){ 
    std::cout << "fn pointer as params:" << selectCmp(&compare2, 8,999) << std::endl;
};

//////////////////// 4. 返回函数指针

//表示一个 getCmp(const int &type) 函数，返回值类型是 bool (*cmp)(const int &x, const int &y) 这个函数指针
bool (*getCmp(const int &type))(const int &x, const int &y){
    switch (type)
    {
    case 1:
        return &compare1;
        break;
    case 2:
        return &compare2;
        break;
    default:
        return &compare1;
        break;
    }
};
//用typedef简化的写法
typedef bool (*GET_CMP)(const int &x, const int &y);
GET_CMP getCmp2(){
    return &compare1;
};
GET_CMP getCmp3(const int &i){
    switch (i)
    {
    case 1:
        return &compare1;
        break;
    case 2:
        return &compare2;
        break;
    default:
        return &compare1;
        break;
    }
};

void testPointerAsRet(){
    std::cout << "cmp1:" << getCmp(1)(4,5) << std::endl;
    std::cout << "cmp1:" << getCmp(2)(4,5) << std::endl;
    std::cout << "cmp2:" << getCmp2()(4,5) << std::endl;
    std::cout << "cmp3:" << getCmp3(1)(4,5) << std::endl;
    std::cout << "cmp3:" << getCmp3(2)(4,5) << std::endl;
};

int main(){
    //testUseFnPointer();
    //testPointerAsParams();
    testPointerAsRet();
    return 0;
}