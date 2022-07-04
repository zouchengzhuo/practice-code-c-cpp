#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
//内核的aio实现 需要 yum install -y libaio-devel 以获得此头文件，编译的时候也需要链接动态库 -L/usr/lib64 -laio
//libaio是对内核的aio相关函数的封装
//centos8下可以同步读取可以成功，但是异步读取无法成功
#include <libaio.h>


void LibioCallback(io_context_t ctx, struct iocb *cb, long res, long res2) {
    
    printf("res:%d, res2: %d, nbytes: %d, offset: %d\n", res, res2, cb->u.c.nbytes, cb->u.c.offset);
    printf("buf: %s \n", cb->u.c.buf);
}

/**
 * @brief 使用 io_getevents 阻塞并等待结果
 * 
 * @param ctx 
 * @param max_event 
 */
void CheckByIOGetevents(io_context_t &ctx, const int &max_event){
    ////step3. 阻塞，等待结果，执行回调
    io_event event[max_event];
    int num = io_getevents(ctx, max_event, max_event, event, NULL);
    printf("io_getevents num:%d\n", num);
    //有异步队列读取完了
    for (int i = 0; i < num; i++) {
        //调用回调对象中的回调函数（这里实际上只是打印了下信息）
        io_event &e = event[i];
        io_callback_t io_callback = (io_callback_t)e.obj->data;
        io_callback(ctx, event[i].obj, event[i].res, event[i].res2);
    }
}

/**
 * @brief 异步IO：libaio 内核实现的异步io api，多并发读取
 * @return std::string 
 */
std::string ReadByKernelAIO(){
    const int MAX_EVENT = 5;
    const int SEQ_BUF_SIZE = 1024;
    char f_path[] = "./test.txt";
    //很多文章说kernel aio只支持 O_DIRECT，这里测试 O_RDONLY 也是可以的，O_DIRECT 模式下 io_event.res 的值是-22
    //更新：只有正确初始化堆内存，而且内存对齐时 alignment 使用512，才不会报错
    int fd = open(f_path, O_DIRECT);
    ////step1. 初始化异步io的context
    io_context_t      ctx;
    memset(&ctx, 0, sizeof(ctx));
    if(io_setup(MAX_EVENT, &ctx)){
        printf("io_setup error\n");
        return "";
    }
    ////step2. 创建回调并提交异步任务
    //创建回调对象
    //异步io回调对象，事件触发时会被设置给io_event
    // iocbp->iocb.data 自定义字段，如果用io_set_callback设置了回调函数，那么保存的是回调函数地址；若设置了自定义数据，则保存自定义数据地址。
    // iocbp->iocb.u.c.nbytes 字节数 TODO：这个和 io_event.res 有什么关系？ 看起来都是字节数，但是 res经常返回0或者-22，而nbytes是正常字节数
    // iocbp->iocb.u.c.offset 偏移
    // iocbp->iocb.u.c.buf 缓冲空间
    // iocbp->iocb.u.c.flags 读写
    iocb io_cb[MAX_EVENT];
    iocb *io_cb_p[MAX_EVENT];
    char *buf;
    posix_memalign((void**)&buf, 512, SEQ_BUF_SIZE * MAX_EVENT);
    for (int i = 0; i < MAX_EVENT; i++) {
        io_cb_p[i] = &io_cb[i];
        int offset = i*SEQ_BUF_SIZE;
        io_prep_pread(io_cb_p[i], fd, buf + offset, SEQ_BUF_SIZE, offset);
        io_set_callback(io_cb_p[i], LibioCallback);
    }
    //一次性提交提交多个任务，如果第三个指针参数设置错误，实际上只有第一个地址是指向 iocb 的，那么 io_getevents 总是只能拿到一个event，如果试图拿到更多，就永远阻塞了
    if (io_submit(ctx, MAX_EVENT, io_cb_p) < 0) {
        printf("io_submit error");
        return "";
    }
    //通过 io_getevents 阻塞并等待异步任务完成
    CheckByIOGetevents(ctx, MAX_EVENT);
    io_destroy(ctx);
    close(fd);
    free(buf);
    return "";
}

/**
 * @brief 异步IO：libaio 内核实现的异步io api，用 eventfd + epoll 控制
 * @return std::string 
 */
std::string ReadByKernelAIOAndEventfdAndEpoll(){
    const int MAX_EVENT = 5;
    const int SEQ_BUF_SIZE = 1024;
    char f_path[] = "./test.txt";
    //很多文章说kernel aio只支持 O_DIRECT，这里测试 O_RDONLY 也是可以的，O_DIRECT 模式下 io_event.res 的值是-22
    //更新：只有正确初始化堆内存，而且内存对齐时 alignment 使用512，才不会报错
    int fd = open(f_path, O_DIRECT);
    ////step1. 初始化异步io的context
    io_context_t      ctx;
    memset(&ctx, 0, sizeof(ctx));
    if(io_setup(MAX_EVENT, &ctx)){
        printf("io_setup error\n");
        return "";
    }
    ////step2. 创建回调并提交异步任务
    iocb io_cb[MAX_EVENT];
    iocb *io_cb_p[MAX_EVENT];
    char *buf;
    //创建 epoll fd
    int efd = epoll_create(MAX_EVENT);
    posix_memalign((void**)&buf, 512, SEQ_BUF_SIZE * MAX_EVENT);
    for (int i = 0; i < MAX_EVENT; i++) {
        io_cb_p[i] = &io_cb[i];
        int offset = i*SEQ_BUF_SIZE;
        io_prep_pread(io_cb_p[i], fd, buf + offset, SEQ_BUF_SIZE, offset);
        //创建eventfd
        int evfd = eventfd(0, 0);
        //将eventfd设置给iocb TODO：指针，考虑分配到堆内存
        io_set_eventfd(io_cb_p[i], evfd);
        //添加 eventfd 到epoll的监听中，只监视可读事件
        epoll_event in_ev{EPOLLIN, {.fd=evfd}};
        epoll_ctl(efd, EPOLL_CTL_ADD, evfd, &in_ev);
        //设置回调函数
        io_set_callback(io_cb_p[i], LibioCallback);
    }
    //一次性提交提交多个任务，如果第三个指针参数设置错误，实际上只有第一个地址是指向 iocb 的，那么 io_getevents 总是只能拿到一个event，如果试图拿到更多，就永远阻塞了
    if (io_submit(ctx, MAX_EVENT, io_cb_p) < 0) {
        printf("io_submit error");
        return "";
    }
    //阻塞，监听epoll返回
    epoll_event evs[MAX_EVENT];
    int finished = 0;
    while (true)
    {
        int ret = epoll_wait(efd, evs, MAX_EVENT, -1);
        printf("epoll wait get events: %d\n", ret);
        finished += ret;
        if(ret <=0){
            printf(" epoll fail with ret: %d \n", ret);
            return "";
        }
        //因为只监视了可读事件，此时有ret个eventfd可读了，代表有ret个异步读取任务好了，直接 io_getevents即可
        //通过 io_getevents 阻塞并等待异步任务完成
        CheckByIOGetevents(ctx, ret);
        if(finished >= MAX_EVENT) break;
    }

    io_destroy(ctx);
    close(fd);
    free(buf);
    return "";
}

/**
 * @brief 网络socket的读写也能用aio来管理
 * 
 * @return std::string 
 */
std::string ReadByKernelAIOINET(){

}

/**
 * @brief 【错误1】：先绑定callback，再调用io_prep_pread，会导致callback绑定失效
 * 
 * @return std::string 
 */
void ReadByKernelAIOCallbackError(){
    // io_set_callback(io_cb_p[i], LibioCallback);
    // io_prep_pread(io_cb_p[i], fd, buf, SEQ_BUF_SIZE, offset);
    //// 因为源码是这样实现的
    // static inline void io_prep_pread(struct iocb *iocb, int fd, void *buf, size_t count, long long offset)
    // {
    // 	memset(iocb, 0, sizeof(*iocb));
    // 	iocb->aio_fildes = fd;
    // 	iocb->aio_lio_opcode = IO_CMD_PREAD;
    // 	iocb->aio_reqprio = 0;
    // 	iocb->u.c.buf = buf;
    // 	iocb->u.c.nbytes = count;
    // 	iocb->u.c.offset = offset;
    // }
}

/**
 * @brief 【错误2】：这种写法，由于错用了双重指针，导致 io_submit 提交成功的只有一个任务
 * 此时如果 io_getevents 传入最小事件为1，那么能拿到一个event后面的拿不到，如果传入的最小event数量大于1，则会永久阻塞。。。
 * @return std::string 
 */
std::string ReadByKernelAIOSubmitError1(){
    const int MAX_EVENT = 5;
    const int SEQ_BUF_SIZE = 1024;
    char f_path[] = "./test.txt";
    //很多文章说kernel aio只支持 O_DIRECT，这里测试 O_RDONLY 也是可以的，O_DIRECT 模式下 io_event.res 的值是-22
    //更新：只有正确初始化堆内存，而且内存对齐时 alignment 使用512，才不会报错
    int fd = open(f_path, O_DIRECT);
    ////step1. 初始化异步io的context
    io_context_t      ctx;
    memset(&ctx, 0, sizeof(ctx));
    if(io_setup(MAX_EVENT, &ctx)){
        printf("io_setup error\n");
        return "";
    }
    ////step2. 创建回调并提交异步任务
    //创建回调对象
    iocb io_cb[MAX_EVENT];
    iocb *io_cb_ptr = &io_cb[0];

    for (int i = 0; i < MAX_EVENT; i++) {
        int offset = i*SEQ_BUF_SIZE;
        void *buf;
        posix_memalign((void**)&buf, 512, SEQ_BUF_SIZE);
        io_prep_pread(io_cb_ptr+i, fd, buf, SEQ_BUF_SIZE, offset);
        io_set_callback(io_cb_ptr+i, LibioCallback);
    }
    //一次性提交提交多个任务，如果第三个指针参数设置错误，实际上只有第一个地址是指向 iocb 的，那么 io_getevents 总是只能拿到一个event，如果试图拿到更多，就永远阻塞了
    if (io_submit(ctx, MAX_EVENT, &io_cb_ptr) < 0) {
        printf("io_submit error\n");
        return "";
    }
    //通过 io_getevents 阻塞并等待异步任务完成
    CheckByIOGetevents(ctx, MAX_EVENT);
    io_destroy(ctx);
    close(fd);
    return "";
}

/**
 * @brief 【错误3】：callback放在局部变量里边，添加的循环结束后即被覆盖
 * 此时栈内存如果没被其它数据覆盖，会表现为能拿到5个event，但是offset和数据都是最后一个的
 * 如果栈内存已经被覆盖，则程序可能core掉
 * @return std::string 
 */
std::string ReadByKernelAIOSubmitError2(){
    const int MAX_EVENT = 5;
    const int SEQ_BUF_SIZE = 1024;
    char f_path[] = "./test.txt";
    //很多文章说kernel aio只支持 O_DIRECT，这里测试 O_RDONLY 也是可以的，O_DIRECT 模式下 io_event.res 的值是-22
    //更新：只有正确初始化堆内存，而且内存对齐时 alignment 使用512，才不会报错
    int fd = open(f_path, O_DIRECT);
    ////step1. 初始化异步io的context
    io_context_t      ctx;
    memset(&ctx, 0, sizeof(ctx));
    if(io_setup(MAX_EVENT, &ctx)){
        printf("io_setup error\n");
        return "";
    }
    ////step2. 创建回调并提交异步任务
    for (int i = 0; i < MAX_EVENT; i++) {
        //创建回调对象
        iocb io_cb;
        iocb *io_cb_ptr = &io_cb;
        int offset = i*SEQ_BUF_SIZE;
        void *buf;
        posix_memalign((void**)&buf, 512, SEQ_BUF_SIZE);
        io_prep_pread(io_cb_ptr, fd, buf, SEQ_BUF_SIZE, offset);
        io_set_callback(io_cb_ptr, LibioCallback);
        if(io_submit(ctx, 1, &io_cb_ptr) < 0) {
            printf("io_submit error\n");
            return "";
        }
    }
    //通过 io_getevents 阻塞并等待异步任务完成
    CheckByIOGetevents(ctx, MAX_EVENT);
    io_destroy(ctx);
    close(fd);
    return "";
}

/**
 * @brief 【错误4】： 内存分配方式错误，这种方式 offset 没问题，但是内容都是最后一段的
 * 1.需要保证内存分配在堆内存中，而不是局部的栈内存，不然有可能被覆盖
 * 2.O_DIRECT需要调用 posix_memalign 来进行分配，且与官方的描述不同，只有在特定的  alignment （centos8下，512字节可以，256不行） 下才不报-22（即-EINVAL）
 * 3.io_prep_pread的时候，传入的读取buf size 也必须是512/1024这样的2的n次方，否则 nbytes 正确，但是 res 会报-22，且拿不到buf结果
 * @return std::string 
 */
std::string ReadByKernelAIOAllocError(){
    const int MAX_EVENT = 5;
    const int SEQ_BUF_SIZE = 1024;
    char f_path[] = "./test.txt";
    int fd = open(f_path, O_RDONLY);
    ////step1. 初始化异步io的context
    io_context_t      ctx;
    memset(&ctx, 0, sizeof(ctx));
    if(io_setup(MAX_EVENT, &ctx)){
        printf("io_setup error\n");
        return "";
    }
    ////step2. 创建回调并提交异步任务
    //创建回调对象
    iocb io_cb[MAX_EVENT];
    iocb *io_cb_p[MAX_EVENT];

    //注意点1：这种初始化方式，每个callback 都往这块栈内存里边写，会覆盖内容
    char buf[SEQ_BUF_SIZE];

    for (int i = 0; i < MAX_EVENT; i++) {
        io_cb_p[i] = &io_cb[i];
        int offset = i*SEQ_BUF_SIZE;
        //注意点2：写到里边来每次分配新的栈内存，也不行，局部变量作用域结束后仍然会被覆盖
        //char buf[SEQ_BUF_SIZE];
        //注意点3：先在栈空间分配，再调用posix_memalign做对齐内存分配，是无效的操作，并不会去对齐分配，需要声明  void *buf; 或者 char *buf; 再对齐内存
        //posix_memalign((void**)&buf, 512, SEQ_BUF_SIZE);
        io_prep_pread(io_cb_p[i], fd, buf, SEQ_BUF_SIZE, offset);
        io_set_callback(io_cb_p[i], LibioCallback);
    }
    //一次性提交提交多个任务，如果第三个指针参数设置错误，实际上只有第一个地址是指向 iocb 的，那么 io_getevents 总是只能拿到一个event，如果试图拿到更多，就永远阻塞了
    if (io_submit(ctx, MAX_EVENT, io_cb_p) < 0) {
        printf("io_submit error");
        return "";
    }
    //通过 io_getevents 阻塞并等待异步任务完成
    CheckByIOGetevents(ctx, MAX_EVENT);
    io_destroy(ctx);
    close(fd);
    return "";
}


/**
 * @brief 异步IO：libaio 内核实现的异步io api  异步批量读取
 * @return std::string 
 */
std::string BatchReadByLibAIO(){
    const int MAX_EVENT = 5;
    //一次读取1k字节的数据
    const int SEQ_BUF_SIZE = 1024;
    const int ALIGNMENT = 512;
    char f_path[] = "./test.txt";
    int fd = open(f_path, O_DIRECT);
    //获取文件尺寸，以存放文件内容
    struct stat f_stat;
    stat(f_path, &f_stat);
    size_t f_size = f_stat.st_size;
    printf("file size: %d\n", f_size);
    //用于存放文件内容的 buffer，需要改成1024的整数倍，不然内存对齐会失败（因为要使用 O_DIRECT 模式）
    size_t align_size = ceil((float)f_size/(float)SEQ_BUF_SIZE) * SEQ_BUF_SIZE;
    char *buf;
    posix_memalign((void**)&buf, ALIGNMENT, align_size);
    printf("align_size: %d sizeof(void*): %d\n", align_size, sizeof(void*));
    //初始化异步io的context
    io_context_t      ctx;
    memset(&ctx, 0, sizeof(ctx));
    if(io_setup(MAX_EVENT, &ctx)){
        printf("io_setup error\n");
        return "";
    }
    iocb *io_cb = (iocb*)malloc(sizeof(iocb)*MAX_EVENT);
    //需要一块连续的双重指针数组存储空间 io_cb_p ，不然 io_submit 的时候无法从连续空间里边读到所有的异步任务，只能读到第一个
    iocb *io_cb_p[MAX_EVENT];
    size_t offset = 0, ready = 0;
    //按 MAX_EVENT 数量，创建对应数量的异步IO任务
    int task_no = 0;
    for(;task_no<MAX_EVENT;task_no++){
        io_cb_p[task_no] = &io_cb[task_no];
        //当前任务读取的字节数量
        size_t count = SEQ_BUF_SIZE;
        //size_t count = (offset + SEQ_BUF_SIZE) > f_size ? (f_size - offset) : SEQ_BUF_SIZE; //会导致最后一个包报-22，读取不全
        io_prep_pread(io_cb_p[task_no], fd, buf + offset, count, offset);
        //不设置回调了
        //io_set_callback(&io_cb[task_no], LibioCallback);
        if(offset + SEQ_BUF_SIZE < f_size){
             offset += SEQ_BUF_SIZE;
        } else {
            break;
        }
    }
    //提交异步IO任务
    //if (io_submit(ctx, task_no, &io_cb) < 0) { //这样不行：不是连续的双重指针地址，只能submit第一个异步任务，导致无限阻塞
    if (io_submit(ctx, task_no, io_cb_p) < 0) { //这样可以，是连续的双重指针地址
        printf("io_submit error");
        return "";
    }
    while (1)
    {
        //阻塞，等待
        io_event event[MAX_EVENT];
        printf("start get events\n");
        int num = io_getevents(ctx, 1, MAX_EVENT, event, NULL);
        printf("io_getevents num %d offset %d\n", num, offset);
        //有异步队列读取完了
        for (int i = 0; i < num; i++) {
            //调用回调对象中的回调函数（这里实际上只是打印了下信息）
            io_event &e = event[i];
            printf("get event, nbytes： %d res:%d res2: %d c.offset: %d\n",  e.obj->u.c.nbytes, e.res, e.res2, e.obj->u.c.offset );
            //若读取还没有完毕，每完成一个异步任务就添加一个新的异步任务。这种方式会产生大量的用户态-内核态上下文切换，仅作练习使用
            //如果offset没超过文件大小，说明文件没读完，继续加异步任务
            if(offset < f_size){
                //注意，这里不能分配在栈内存里，不然局部变量作用域结束后，异步任务处理过程中内存可能会被覆盖
                iocb *new_io_cb = (iocb*)malloc(sizeof(iocb));//todo：这里可能会泄露，考虑在外面回收
                size_t count = SEQ_BUF_SIZE;
                //size_t count = (offset + SEQ_BUF_SIZE) > f_size ? (f_size - offset) : SEQ_BUF_SIZE;//会导致最后一个包报-22，读取不全
                io_prep_pread(new_io_cb, fd, buf + offset, count, offset);
                printf("【continue】 add task offset: %d count: %d\n", offset, count);
                if (io_submit(ctx, MAX_EVENT, &new_io_cb) < 0) {
                    printf("io_submit error");
                    return "";
                }
                offset += e.obj->u.c.nbytes;
            }
            ready += e.obj->u.c.nbytes;
        }
        printf("ready %d offset %d\n", ready, offset);
        //如果已经ready的字节大于等于文件大小，说明读完了，结束任务
        if(ready >= f_size) break;
    }

    std::string s(buf, buf + f_size);
    printf("===== s =====\n%s\n", s.c_str());
    io_destroy(ctx);
    close(fd);
    free(io_cb);
    free(buf);
    return s;
}

//相关资料：
// Linux aio：linux可扩展性项目的子项目：官网http://lse.sourceforge.net/io/aio.html
// libaio：是aio 系统调用的封装：官网https://pagure.io/libaio/tree/master

// 根据官网：these do not return an explicit error, but quietly default to synchronous or rather non-AIO behaviour
// 及最新文章https://blog.cloudflare.com/io_submit-the-epoll-alternative-youve-never-heard-about/
// 目前的aio还是不支持buffer io，你的程序正常是变成同步了，其实还是阻塞在了io_submit。

// libaio + epoll:
// https://support.huaweicloud.com/tuningtip-kunpenggrf/kunpengtuning_12_0069.html
// http://www.xmailserver.org/eventfd-aio-test.c

// demo https://github.com/littledan/linux-aio
