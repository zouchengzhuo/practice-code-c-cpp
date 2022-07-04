#include <stdio.h>      // printf
#include <fcntl.h>      // open
#include <unistd.h>     // read, close, getpagesize
#include <sys/mman.h>   // mmap
#include <string.h>     // memcmp, strlen
#include <assert.h>     // assert
#include <iostream>

/**
 * @brief 测试往虚拟内存中写，从设备文件中读；与往设备文件中写，从虚拟内存中读
 * 
 */
void TestReadAndWrite(){

    char buf[64];
    int fd;
    char *addr = NULL;
    int ret;
    char f_path[] = "./test1.txt";
    char message[] = "Hello World\n";
    char message2[] = "I'm superman\n";
    //打开目标文件
    fd = open(f_path, O_RDWR);
    //一个空文件，得扩一下大小，不然mmap映射无效
    fallocate( fd, 0, 0, 64);
    if (fd < 0) {
        printf("open device %s failed\n", f_path);
        return;
    }
    
    sleep(5);
    //建立内存映射
    addr = (char*)mmap(NULL, (size_t)getpagesize(), PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_LOCKED, fd, 0);
    sleep(5);
    /* 测试映射正确 */
    /* 写到mmap映射的虚拟地址中，通过read读取设备文件 */
    ret = sprintf(addr, "%s", message);
    //判断是否完全写入
    assert(ret == strlen(message));
    ret = read(fd, buf, 64);
    //判断是否能完整读出
    assert(ret == 64);
    //比对从设备文件读出来的字符，和message是否能对得上
    assert(!memcmp(buf, message, strlen(message)));

    /* 通过write写入设备文件，修改体现在mmap映射的虚拟地址 */
    ret = write(fd, message2, strlen(message2));

    assert(ret == strlen(message2));
    assert(!memcmp(addr + 64, message2, strlen(message2)));

    munmap(addr, (size_t)getpagesize());
    close(fd);
}

/**
 * @brief 映射同一个文件，不同进程把同一个设备文件映射到不同的用户态地址空间
 * 
 */
void TestRead2(){
    char buf[64];
    int fd;
    char *addr = NULL;
    int ret;
    char *message = "Hello World\n";
    char *message2 = "I'm superman\n";
    char f_path[] = "./test1.txt";

    /* 另一进程打开同一设备文件，然后用mmap映射 */
    fd = open(f_path, O_RDWR);
    if (fd < 0) {
        printf("open device %s failed\n", f_path);
        return;
    }
    addr = (char*)mmap(NULL, (size_t)getpagesize(), PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_LOCKED, fd, 0);

    /* 通过read读取设备文件 */
    ret = read(fd, buf, sizeof(buf));

    assert(ret == sizeof(buf));
    assert(!memcmp(buf, message, strlen(message)));

    /* 通过mmap映射的虚拟地址读取 */
    assert(!memcmp(addr + sizeof(buf), message2, strlen(message2)));

    munmap(addr, (size_t)getpagesize());
    close(fd);
    return;
}

/**
 * @brief 映射的大小改成了1个字节，根据前面的代码分析，映射是需要页对齐的，所以预期实际会映射一个页。
 * 在一页的范围内是可以正常读写的。然后尝试写到vma映射范围之外，预期会出现段错误
 */
void TestSegmentError(){
    char buf[64];
    int fd;
    char *addr = NULL;
    off_t offset;
    int ret;
    char *message = "Hello World\n";
    char *message2 = "I'm superman\n";
    char f_path[] = "./test1.txt";

    fd = open(f_path, O_RDWR);
    if (fd < 0) {
        printf("open device %s failed\n", f_path);
        return;
    }
    /* 映射1个字节 */
    addr = (char*)mmap(NULL, 1, PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_LOCKED, fd, 0);

    /* 写到mmap映射的虚拟地址中，通过read读取设备文件 */
    ret =sprintf(addr, "%s", message);
    assert(ret == strlen(message));

    ret = read(fd, buf, sizeof(buf));
    assert(ret == sizeof(buf));
    assert(!memcmp(buf, message, strlen(message)));

    /* 写到一页的尾部 */
    ret = sprintf(addr + getpagesize() - sizeof(buf), "%s", message2);
    assert(ret == strlen(message2));

    offset = lseek(fd, getpagesize() - sizeof(buf), SEEK_SET);
    assert(offset == getpagesize() - sizeof(buf));

    ret = read(fd, buf, sizeof(buf));
    assert(ret == sizeof(buf));
    assert(!memcmp(buf, message2, strlen(message2)));
    
    /* 写到一页之后，超出映射范围 */
    printf("expect segment error\n");
    ret = sprintf(addr + getpagesize(), "something");
    printf("never reach here\n");

    munmap(addr, 1);
    close(fd);
    return;
}