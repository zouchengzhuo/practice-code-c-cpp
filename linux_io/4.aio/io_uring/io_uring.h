#include <liburing.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void ReadByIOUring(){
    //打开文件，读取大小等准备工作
    char f_path[] = "./test.txt";
    int f_fd = open(f_path, O_RDONLY | O_DIRECT);
    int offset = 0;
    // struct stat f_stat;
    // stat(f_path, &f_stat);
    // size_t f_size = f_stat.st_size;

    //1.初始化 iouring 示例
    io_uring ring;
    const size_t QUEUE_ENTRIES_SIZE = 5; //共享队列长度
    int flags = 0; //0 表示默认配置，例如使用中断驱动模式。参数描述见：https://man.archlinux.org/man/io_uring_setup.2.en 

    int ret = io_uring_queue_init(QUEUE_ENTRIES_SIZE, &ring, IORING_SETUP_IOPOLL);    // flags，0 表示默认配置，例如使用中断驱动模式
    //这里会跪，报-12：ENOMEM 12	/* Out of memory */
    printf("io_uring_queue_init:%d\n", ret);
    
    //2.初始化缓冲区
    char *buf;
    const size_t SEQ_BUF_SIZE = 1024;
    posix_memalign((void**)&buf, 512, SEQ_BUF_SIZE*SEQ_BUF_SIZE);
    io_uring_get_sqe(&ring); 
    //3.准备 QUEUE_ENTRIES_SIZE 个 submit queue 读请求，指定将随后读入的数据写入 iovecs 
    struct io_uring_sqe *sqe;
    for (int i = 0; i < QUEUE_ENTRIES_SIZE; i++) {
        sqe = io_uring_get_sqe(&ring);  // 获取可用的 submit queue entry
        io_uring_prep_read(sqe, f_fd, (char*)buf + i*SEQ_BUF_SIZE, 1, i*SEQ_BUF_SIZE);
    }
    //4.提交前面准备的 submit queue entries，返回提交成功的数量
    int entries_num = io_uring_submit(&ring);
    if (ret < 0) {
        fprintf(stderr, "io_uring_submit: %s\n", strerror(-ret));
        return;
    }

    //4.等待读请求完成
    struct io_uring_cqe *completion_queue_entry;
    for (int i = 0; i < entries_num; i++) {
        // 等待系统返回一个读完成事件
        io_uring_wait_cqe(&ring, &completion_queue_entry);
        printf("completion_queue_entry->res: %d\n", completion_queue_entry->res);
        // 更新 io_uring 实例的完成队列
        io_uring_cqe_seen(&ring, completion_queue_entry);
    }
     printf("buf: %s\n", buf);
    // 5. 清理工作
    close(f_fd);
    io_uring_queue_exit(&ring);
    free(buf);
    return;
}