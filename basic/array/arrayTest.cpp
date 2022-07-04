#include "arrayTest.h"

void ArrayTester::testInit(){
    std::cout << "================= TestInit start =================" << std::endl;
    const int len = 10;
    int len2;
    //不能通过变量初始化长度
    int arr1[9]; //通过数字字面量常量初始化
    int arr2[len];//通过const常量初始化
    //int arr3[len2]; //错误，不能通过变量初始化
    int arr3[3] = {4,6,7}; //显示初始化
    int arr4[] = {7,8,9}; //显示初始化不需要指定长度
    std::cout << "sizeof(arr2):" << sizeof(arr2) << std::endl;//sizeof获取内存长度，4*10=40
    std::cout << "arr4:" << *arr4 << std::endl; //数组变量名实际上是个指针类型，指向数组的第一个元素

    //字符类型数组有个特殊的场景，可以用字符串字面量来初始化数组，但是要注意最后面会自动添加一个null
    char arr5[] = "abc"; 
    //char arr6[3] = "abc"; //会报错，因为忽略了末尾null的长度，数组长度没有分配够
    char arr7[4] = "abc"; 
    std::string str(arr7);
    std::cout << "arr5:" << str << std::endl;
    std::cout << "================= TestInit end =================" << std::endl;
};

void ArrayTester::testDynamicArray(){
    std::cout << "================= testDynamicArray start =================" << std::endl;
    //c用malloc和free来分配、释放内存， cpp里边的new和delete做同样的事情
    int *arr1 = new int[10]; //内置类型：new，不带小括号，不会给初始化(实际上也会读出个0)
    int *arr2 = new int[10](); //带小括号，会给初始化，全0
    std::string *arr3 = new std::string[10]; //外部类型，不带小括号也会给调用构造函数，初始化

    //特殊：const类型的动态数组，不能修改元素，所以内置类型的必须带小括号给初始化
    //const int *arr4 = new const int[10]; //错误： const类型数组不能修改，所以必须初始化
    const int *arr4 = new const int[10](); //正确
    const std::string *arr5 = new const std::string[10]; //正确，自定义类型会自动调构造函数

    //特殊：动态数组可以在运行时获取长度
    std::srand(std::time(NULL));
    int size = std::rand() % 100;
    int *arr6 = new int[size];
    std::cout << "random size:" << size << std::endl;
    
    //一定注意释放，否则会内存泄露
    delete [] arr1;
    delete [] arr2;
    delete [] arr3;
    delete [] arr4;
    delete [] arr5;
    delete [] arr6;

    //用途：
    //例如运行时动态生成的字符串，要用cstring的api来操作赋值给新的数组，为了防止长度溢出，可以使用动态数组来装，如：
    const char *err;
    const char *err1 = "errmsg1", *err2 = "errmsg2: xxx test reason";
    int rand = std::rand();
    if(rand% 2 ==0){
        err = err1;
    } else {
        err = err2;
    }
    //动态数组做长度保护：
    int esize = std::strlen(err);
    char *fullerr = new char[esize];
    //sizeof操作符，获取数据长度
    //size_t fullSize = sizeof(*fullerr); //错误，这样得到的是指向数组第一个元素的字节长度
    size_t fullSize = sizeof(fullerr);    //错误，获取的是存储指针的内存尺寸
    size_t charSize = sizeof(char);
    //结论：动态数组无法通过sizeof获取正确长度
    std::cout << "array length:" << fullSize / charSize << " esize:" << esize << " fullSize:" << fullSize << std::endl;
    
    std::strcpy(fullerr, err);
    delete [] fullerr;
    std::cout << "random error:" << fullerr << std::endl;
    std::cout << "================= testDynamicArray end =================" << std::endl;
};

void ArrayTester::testMultiArray(){
    int arr1[3][4] = {{0,1,2,3}, {4,5,6,7}, {8,9,10,11}};
    int arr2[3][4] = {0,1,2,3, 4,5,6,7, 8,9,10,11}; //两者等价
    //不好记的地方来了，如何定义指向arr1的指针
    int (*p1)[4] = arr1; //注释里可以看到 arr1是 a value of type "int (*)[4]"，所以要这样写
    p1 = &arr1[2]; //arr1共三个元素，修改到最后一个
    std::cout << "p1[0][0]:" << p1[0][0] << std::endl;

    int *p2[4] = {&arr1[1][0], &arr1[1][1], &arr1[1][2], &arr1[1][3]}; //一个长度为4的int型指针数组， 与 (*p1)[4]区分开
    std::cout << "p2[0]:" << *p2[0] << std::endl;
    
    //用typedef简化多维数组指针的写法
    typedef int int_arr4[4];
    int_arr4 *p3 = arr1;
    std::cout << "p3[0][0]:" << p3[0][0] << std::endl;
};

int main(){
    ArrayTester tester;
    tester.testInit();
    //tester.testDynamicArray();
    //tester.testMultiArray();
    return 0;
}