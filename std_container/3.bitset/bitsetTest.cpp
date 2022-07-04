#include "bitsetTest.h"

void BitsetTester::TestInit(){
    std::cout << "================= TestInit start =================" << std::endl;
    std::bitset<16> b1; //16位，全0
    std::bitset<16> b2(0xffff);//从数字初始化，16位，全1
    std::bitset<32> b3(0xffff);//从数字初始化，32位，前16位为1，后16位为0
    std::string s1 = "10100101";
    std::bitset<8> b4(s1);//从字符串初始化，这里要注意只能从std::string初始化，不能从"10100101"这种字符串字面量初始化
    std::bitset<4> b5(s1, 3,4);//从字符串初始化，s,pos,n，从位置3开始，取长度为4的字符串赋值
    // std::string s2 = "kkkk"; //必须是01组成的字符串，否则会core
    // std::bitset<8> b6(s2);
    std::cout << "b1:" << b1 << std::endl;
    std::cout << "b2:" << b2 << std::endl;
    std::cout << "b3:" << b3 << std::endl;
    std::cout << "b4:" << b4 << std::endl;
    std::cout << "b5:" << b5 << std::endl;
    std::cout << "================= TestInit end =================" << std::endl;
};

void BitsetTester::TestOp(){
    std::cout << "================= TestOp start =================" << std::endl;
    std::bitset<32> b(0xff00ffee);
    std::cout << "b:" << b << std::endl; //可直接用cout输出
    std::cout << "b.any():" << b.any() << std::endl; //是否存在一位为1
    std::cout << "b.none():" << b.none() << std::endl; //是否不存在任何一位为1
    std::cout << "b.count():" << b.count() << std::endl; //为1的位数量
    std::cout << "b.size():" << b.size() << std::endl; //总位数
    //// 注意：打印出来，第0位是在最右侧，按数字的读法来，不是按字符串/数组的读法来
    b[4] = 1; //操作某一位
    std::cout << "b[4] = 1:" << b << std::endl; //修改后结果
    std::cout << "b.test(4):" << b.test(4) << std::endl; //某一位是否为1
    b.set(0);//把某一位设置位1
    std::cout << "b.set(0):" << b << std::endl; //修改后结果
    b.reset(2);//把某一位设置为0
    std::cout << "b.reset(2):" << b << std::endl; //修改后结果
    b.flip(0);//把某一位的值取反
    std::cout << "b.flip(0):" << b << std::endl; //修改后结果
    unsigned long l = b.to_ulong();
    std::cout << "b.to_ulong:" << l << std::endl; //修改后结果
    b.flip();//取反所有位
    std::cout << "b.flip():" << b << std::endl; //修改后结果
    b.set(); //所有位设置为1
    std::cout << "b.set():" << b << std::endl; //修改后结果
    b.reset();//所有位设置为0
    std::cout << "b.reset():" << b << std::endl; //修改后结果

    //支持位运算
    b |= 0xffff;
    std::cout << "b |= 0xffff:" << b << std::endl; //修改后结果
    b <<= 2;
    std::cout << "b <<= 2:" << b << std::endl; //修改后结果
    std::cout << "================= TestOp end =================" << std::endl;
}

int main(){
    BitsetTester tester;
    tester.TestInit();
    tester.TestOp();
    return 0;
}