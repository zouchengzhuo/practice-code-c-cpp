#include <stdio.h>
#include <iostream>
#include <string.h>

/**
 * @brief 同步IO：通过c标准库glibc的 fopen & fgets & fclose 调用， 通过文件指针 FILE* 来操作文件，底层也是基于系统函数 open 来实现的
 * 与直接open、read相比，最重要的区别是，read每次调用都要在用户态→内核态中切换；
 * 而 fopen会在用户态创建一个缓冲区，一次性将一个默认大小（一般是一个内核高速缓存页的大小，如4096）的内容拷到用户态缓冲区，每次fgets的时候就不涉及切换了
 * 可以通过 setvbuf 修改这个缓冲区的大小
 * @return std::string 
 */
std::string ReadByFOpen(){
    FILE* fp= fopen("./test.txt", "r");
    if(fp == nullptr) return "";
    const int BUF_SIZE = 1024;
    char buf[BUF_SIZE];
    int size = 0;
    std::string s;
    while( fgets(buf, BUF_SIZE, fp) != NULL ) {
        //貌似也不需要每次reset，fgets每次读到换行符就结束，而且strlen在最后一波读到结束符就会终止计数
        //memset(buf, 0, BUF_SIZE);
        //printf("buf size: %d\n", strlen(buf));
        //与open+read相比，这里每次只读取了一行，而open+read会读取满指定的buf大小
        s.append(buf, buf+strlen(buf));
    }
    fclose(fp);
    printf("%s\n", s.c_str());
    return s;
}