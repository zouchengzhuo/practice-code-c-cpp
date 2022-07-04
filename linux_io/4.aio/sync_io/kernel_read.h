#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>

/**
 * @brief 同步IO：通过linux系统调用 open & read 读取完整文件内容
 * 从磁盘读取到内核的高速缓存（若高速缓存中已经有了，直接读取），然后从内核读取到用户空间buffer，通过设置flag o_direct 可以跳过内核缓存直接读取
 * @return std::string 
 */
std::string ReadBySyscallOpen(){
    int fd = open("./test.txt", O_RDONLY);
    const int BUF_SIZE = 1024;
    char buf[BUF_SIZE];
    int size = 0;
    std::string s;
    do{
        size = read(fd, buf, BUF_SIZE);
        if(size <= 0) break;
        s.append(buf, buf + size);

    } while (1);
    //printf("%s\n", s.c_str());
    close(fd);
    return s;
}

/**
 * @brief pread：只读取数据，不移动指针
 * 
 */
void PreadBySyscallOpen(){
    int fd = open("./test.txt", O_RDONLY);
    const int BUF_SIZE = 10;
    char buf[BUF_SIZE];
    int size = 0;
    for(int i=0;i<5;i++){
        size = pread(fd, buf, BUF_SIZE, 0);
        if(size <= 0) break;
        printf("pread: %s\n", buf);
    }
    close(fd);
}
/**
 * @brief readv：读取数据到多个buf中，一个填满了就下一个
 * 
 */
void ReadvBySyscallOpen(){
    int fd = open("./test.txt", O_RDONLY);
    const int BUF_SIZE = 10;
    const int SEQ = 5;
    struct iovec iovecs[SEQ];
    for(int i=0; i<SEQ; i++){
        iovecs[i].iov_base = calloc(1, BUF_SIZE*(i+1));
        iovecs[i].iov_len = BUF_SIZE*(i+1);
    }
    readv(fd, iovecs, SEQ);
    for(int i=0;i<SEQ;i++){
        printf("readv: %s\n", iovecs[i].iov_base);
    }
    close(fd);
}

/**
 * @brief preadv：读取数据到多个buf中，一个填满了就下一个，不移动指针
 *  这里有一点要注意的，同一次 preadv 读到的多个buf，还是内容顺序往前读的，只有多次调用 preadv 的时候，文件指针才是不变的
 */
void PreadvBySyscallOpen(){
    int fd = open("./test.txt", O_RDONLY);
    const int BUF_SIZE = 10;
    const int SEQ = 5;
    {
        struct iovec iovecs[SEQ];
        for(int i=0; i<SEQ; i++){
            iovecs[i].iov_base = calloc(1, BUF_SIZE*(i+1));
            iovecs[i].iov_len = BUF_SIZE*(i+1);
        }
        preadv(fd, iovecs, SEQ, 0);
        //多个 iovec 里边的 buf 数据还是不一样
        for(int i=0;i<SEQ;i++){
            printf("readv: %s\n", iovecs[i].iov_base);
        }
    }
    //再次调用 preadv，数据又从头开始读了
    {
        struct iovec iovecs[SEQ];
        for(int i=0; i<SEQ; i++){
            iovecs[i].iov_base = calloc(1, BUF_SIZE*(i+1));
            iovecs[i].iov_len = BUF_SIZE*(i+1);
        }
        preadv(fd, iovecs, SEQ, 0);
        for(int i=0;i<SEQ;i++){
            printf("readv: %s\n", iovecs[i].iov_base);
        }
    }
    close(fd);
}