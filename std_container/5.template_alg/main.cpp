#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>

//测试插入迭代器（back_inserter 后插、front_inserter 前插，inserter 带位置插）
//可以很放的向容器里插入元素，但是可读性堪忧？
void testBackInserter(){
    int a[3] = {1, 2, 3};
    std::vector<int> v1(a, a+3);
    std::cout << "v1 size:" << v1.size() << std::endl;
    //这里有个坑，<std::vector<int> >可以， <std::vector<int>>会报错， >>不能连着写，编译器会报错
    std::back_insert_iterator<std::vector<int> >  insert_iter = std::back_inserter(v1);
    //fill_n(insert_iter, 99, 10); //直接插入99个10
    insert_iter = 4;
    insert_iter = 5;
    std::back_inserter(v1) = 6; //这样写也行
    std::cout << "v1 size:" << v1.size() << std::endl;
}

//测试io对象的迭代器，必须写上模板类型
//如果是自定义类，要用 in迭代器，必须实现 << 运算符， 要用out迭代器，必须实现 >>
void testInteratorOfIO(){
    //写入流迭代器
    //eof用来标记写入流迭代器的end
    std::istream_iterator<int> it(std::cin), eof;
    std::ifstream f_in;
    f_in.open("in_f.txt");
    std::istream_iterator<int> it2(f_in);

    //输出流的迭代器
    std::ostream_iterator<int> ot(std::cout, "\n"); //输出到终端
    std::ofstream f_out;
    f_out.open("out_f.txt");
    std::ostream_iterator<int> ot2(f_out, "\n"); //输出到文件

    //close
    //测试从终端输入，用迭代器同时写入终端和文件
    while (it != eof)
    {
        *ot++ = *it;
        *ot2++ = *it;
        f_out.flush();
        it++;
    }
    std::cout << "done!" << std::endl;
    f_in.close();
    f_out.close();

};

void testInitContainerFromIoIter(){
    //测试从终端输入，初始化vector
    std::istream_iterator<int> it3(std::cin), eof2;
    std::vector<int> v1(it3, eof2);
    for(auto iter = v1.begin();iter!=v1.end();iter++){
        std::cout <<" " << *iter << std::endl;
    }
};

int main(){
    testBackInserter();
    //testInteratorOfIO();
    //testInitContainerFromIoIter();
    return 0;
}