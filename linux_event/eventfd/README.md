# eventfd定义  
eventfd() 函数创建一个 “eventfd 对象”，用户态程序可以使用该对象作为事件等待/通知机制，内核可以将事件通知给用户态程序。  
```c
#include <sys/eventfd.h>
int eventfd(unsigned int initval, int flags);
```
# 初始化参数
## initval
eventfd对象包含一个uint64_t的计数器，由内核保存，初始化eventfd时可以由initval参数来初始化这个计数器的值。  

eventfd()调用返回一个新的fd，指向新创建的这个eventfd对象。

## flags
flags参数可以由下面几个选项按位与组合得到，决定eventfd的行为：  
- EFD_CLOEXEC（linux 2.6.27及以后） 将此fd设置为 close-on-exec （调用exec时自动关闭fd）
- EFD_NONBLOCK（linux 2.6.27及以后）将fd设置为非阻塞
- EFD_SEMAPHORE（linux 2.6.30及以后）从eventfd读出类似信号量的数据，见下面关于 read 的描述

# 相关IO方法

## read
read成功后返回8字节整形的长度（即返回8）。读取时，如果提供的buffer小于8个字节，返回-1，则errno设置为 EINVAL 错误。  

read的结果根据eventfd的counter是否为0，以及创建eventfd对象时flag参数是否设置了EFD_SEMAPHORE，有所不同。  

- 如果未设置EFD_SEMAPHORE且counter的值非0，则read返回一个8字节整形，值是counter的值，并且将counter的值设置为0
- 如果设置了EFD_SEMAPHORE且counter的值非0，则read返回一个8字节整形，值是1，并且将counter的值减一
- 如果counter的值是0，则根据flag是否设置了nonblocking，让进程进入阻塞状态或者返回EAGAIN的errno

## write
write方法可以将buffer中的8字节整形数据加到eventfd的counter上。counter上存储的最大值是 unint64-1，即0xfffffffffffffffe。如果相加时超限了，则根据flag是否设置为非阻塞，wirte会导致阻塞或者返回 EAGAIN 的 errno。  

如果提供给write调用的buffer小于8字节，或者尝试写入0xffffffffffffffff，write会返回 EINVAL 错误。  

## poll、select等类似操作
eventfd支持poll、select、epoll等类似操作。  

- 当counter的值大于0时，eventfd是可读的
- 当counter小于0xffffffffffffffff，即至少可以写入一个1而不阻塞时，eventfd是可写的
- 当counter溢出时，select认为eventfd即是可写的又是可读的，poll则会返回 POLLERR 错误。如上所述，write永远不会导致counter溢出。但是，如果 KAIO 子系统执行了 2^64 个 eventfd“信号发布”，则可能会发生溢出（理论上可能，但实际上不太可能）。 如果发生溢出，则 read 将返回该最大 uint64_t 值（即 0xffffffffffffffff）。  

eventfd 文件描述符还支持其他文件描述符多路复用 API：pselect 和 ppoll。

## close
当不再需要文件描述符时，应将其关闭。 当与同一个 eventfd 对象关联的所有文件描述符都已关闭时，内核会释放对象的资源。    

由 eventfd() 创建的文件描述符的副本由 fork 生成的子进程继承。 重复的文件描述符与相同的 eventfd 对象相关联。 除非设置了 close-on-exec 标志，否则 eventfd() 创建的文件描述符将在 execve 中保留。

# 返回值
成功时，eventfd() 返回一个新的 eventfd 文件描述符。出错时，返回值为-1，errno被设置为对应的错误码。  

# 错误码
- EINVAL flags里边指定了不支持的值
- EMFILE 已达到每个进程打开的文件描述符的数量限制
- ENFILE 已达到操作系统打开的文件描述符的数量限制
- ENODEV 无法挂载(内部)匿名inode设备
- ENOMEM 没有足够内存创建新的 eventfd

# 版本兼容性
eventfd() 从linux内核2.6.22及以后的版本可用，从2.8版本开始，glibc提供了支持。eventfd2()系统调用从内核2.6.27开始支持。从2.9版本以后，glibc的eventfd()内部也是基于eventfd2()来实现的。

# 备注说明
在管道仅用于发送事件信号的所有情况下，应用程序都可以使用 eventfd 文件描述符而不是管道（请参阅 pipe(2)）。 eventfd 文件描述符的内核开销远低于管道，并且只需要一个文件描述符（而管道需要两个）。   

当在内核中使用时，eventfd 文件描述符可以提供从内核到用户空间的桥梁，例如，允许诸如 KAIO（内核 AIO）之类的功能向文件描述符发出信号，表明某些操作已完成。   

eventfd 文件描述符的一个关键点是它可以像任何其他文件描述符一样使用 select(2)、poll(2) 或 epoll(7) 进行监视。这意味着应用程序可以同时监视“传统”文件的准备情况和支持 eventfd 接口的其他内核机制的准备情况。 （没有 eventfd() 接口，这些机制无法通过 select(2)、poll(2) 或 epoll(7) 进行多路复用。）    

在进程的 `/proc/[pid]/fdinfo` 目录中，可以通过相应文件描述符的条目查看 eventfd 计数器的当前值。有关更多详细信息，请参阅 proc(5)。    

有两个底层的 Linux 系统调用：eventfd() 和最近的 eventfd2()。 前一个系统调用没有实现 flags 参数。 后一个系统调用实现了上述标志值。 glibc 包装器函数将在可用的地方使用 eventfd2()。

# 示例程序
```cpp
#include <iostream>
#include <sys/eventfd.h>
#include <unistd.h>
#include <inttypes.h> /* Definition of PRIu64 & PRIx64 */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h> /* Definition of uint64_t */

#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

int main(int argc, char *argv[])
{
    int efd;
    uint64_t u;
    ssize_t s;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <num>...\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    //默认阻塞模式，读取时一次性将counter清零，如果已经是零了则让进程阻塞
    //efd = eventfd(0, 0);
    //非阻塞，信号量模式
    efd = eventfd(0, EFD_SEMAPHORE|EFD_NONBLOCK);
    if (efd == -1)
        handle_error("eventfd");

    switch (fork())
    {
    case 0:
        for (int j = 1; j < argc; j++)
        {
            printf("Child writing %s to efd\n", argv[j]);
            u = strtoull(argv[j], NULL, 0);
            /* strtoull() allows various bases */
            s = write(efd, &u, sizeof(uint64_t));
            if (s != sizeof(uint64_t))
                handle_error("write");
        }
        printf("Child completed write loop\n");

        exit(EXIT_SUCCESS);

    default:
        while (1)
        {
            sleep(1);
            printf("Parent about to read, u: %d\n", u);
            s = read(efd, &u, sizeof(uint64_t));
            printf("Parent read s:%d, u:%d\n", s, u);
            if (s != sizeof(uint64_t)){
                if(errno == EAGAIN){
                    exit(EXIT_SUCCESS);
                }
                handle_error("read");
            }
            printf("Parent read %" PRIu64 " (%#" PRIx64 ") from efd\n", u, u);
        }
    case -1:
        handle_error("fork");
    }
}
```