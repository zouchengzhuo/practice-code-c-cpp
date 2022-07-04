#include <iostream>
#include <aio.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/epoll.h>

/**
 * @brief 信号方式判断结束的handler
 * 
 * @param sig 
 * @param info 
 * @param ucontext 
 */
void SignalHandler(int sig, siginfo_t *info, void *ucontext){
    printf("handle sig: %d\n", sig); 
    // handle sig: 10
    // info->si_code:-4
    // info->si_errno:0
    // info->si_signo:10
    std::cout << "info->si_code:" << info->si_code << std::endl;
    std::cout << "info->si_errno:" << info->si_errno << std::endl;
    std::cout << "info->si_signo:" << info->si_signo << std::endl;
    //一定要设置 cb.aio_sigevent.sigev_value.sival_ptr = &cb， 否则拿不到二进制数据
    aiocb *ptr = (aiocb *)info->si_value.sival_ptr;
    printf("p=%d\n", ptr->aio_offset); 
    printf("p=%d\n", ptr->aio_nbytes); 
    printf("p=%d\n", ptr->aio_sigevent.sigev_signo); 
    printf("read=%s\n", (char *)ptr->aio_buf); 
};

void SignalFdHandler(int sig, aiocb *cb, void *ucontext){
    printf("handle sig: %d\n", sig); 
    printf("p=%d\n", cb->aio_offset); 
    printf("p=%d\n", cb->aio_nbytes); 
    printf("p=%d\n", cb->aio_sigevent.sigev_signo); 
    printf("read=%s\n", (char *)cb->aio_buf); 
};

void ThreadHandler(sigval_t val)
{
    struct aiocb *ptr = (struct aiocb *)val.sival_ptr;
    printf("read=%s", (char *)ptr->aio_buf);    
}

/**
 * @brief 通过sleep判断结束
 * 
 * @param cb 
 */
void CheckBySleep(aiocb &cb){
    while (aio_error(&cb) == EINPROGRESS){
        sleep(1);
    }
    fprintf(stdout, "%s\n", (char*)(cb.aio_buf));
}

/**
 * @brief 通过信号判断结束
 * @param cb 
 */
void CheckBySignal(aiocb &cb){
    struct sigaction sa; 
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = SignalHandler;
    sigaction(SIGUSR1, &sa, NULL);

    //填sigevent指定为信号方式
    cb.aio_sigevent.sigev_value.sival_ptr = &cb;//这个必须要填写，会在siginfo_t中以siginfo_t->si_value.sival_ptr指针返回
    cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    cb.aio_sigevent.sigev_signo = SIGUSR1;
}

/**
 * @brief 通过线程的方式判断结束
 * @param cb 
 */
void CheckByThread(aiocb &cb){
    cb.aio_sigevent.sigev_value.sival_ptr = &cb;
    cb.aio_sigevent.sigev_notify = SIGEV_THREAD;
    cb.aio_sigevent.sigev_notify_function = ThreadHandler;
    cb.aio_sigevent.sigev_notify_attributes = NULL; 
}

/**
 * @brief 通过kqueue的方式判断结束（仿ngiix）
 * kqueue在 BSD (FreeBSD / OpenBSD) and Darwin (Mac OS X / iOS) 内核中才有，linux里边没有，这里不做实验了
 * @param cb 
 */
void CheckByKQueue(aiocb &cb){
    //int kqfd = kqueue();
}

/**
 * @brief 通过信号fd读写来监听结束
 * https://man7.org/linux/man-pages/man2/signalfd.2.html
 * https://zhuanlan.zhihu.com/p/418256266
 * @param cb 
 */
void CheckBySignalFd(aiocb &cb){
    //指定用信号的方式触发
    cb.aio_sigevent.sigev_value.sival_ptr = &cb;//这个必须要填写，会在signalfd_siginfo中以ssi_ptr指针返回
    cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    cb.aio_sigevent.sigev_signo = SIGUSR1;
    //创建 signalfd
    sigset_t mask;
    // 信号清零
    sigemptyset(&mask);
    // 添加信号到掩码集
    sigaddset(&mask, SIGUSR1);
    // 设置该进程为对应的信号集的内容（当前已经的信号集合做并集、交集、覆盖）
    // 这行代码才是真正的信号设置；
    sigprocmask(SIG_BLOCK, &mask, NULL);

    // 创建 signalfd 句柄（绑定信号）
    int sfd = signalfd(-1, &mask, 0);
    for (;;) {
        // 阻塞读取 signalfd 数据（数据代表信号）
        signalfd_siginfo sig;
        size_t s = read(sfd, &sig, sizeof(struct signalfd_siginfo));
        // ...
        // 信号的逻辑处理
        if(s > 0){
            SignalFdHandler(sig.ssi_signo, (aiocb*)sig.ssi_ptr, nullptr);
        } else {
            printf("get signal size %d\n", s);
        }
    }
}

/**
 * @brief 将 posix aio 的 signal 模式下的 signal 转换为 signalfd，这个fd可以用epoll来监听
 * 普通的ext4文件系统的文件不能用 epoll 来监听，因为没有提供poll接口，参考 https://cloud.tencent.com/developer/article/1835294
 * @param cb 
 */
void CheckByEpoll(aiocb &cb){
    //指定用信号的方式触发
    cb.aio_sigevent.sigev_value.sival_ptr = &cb;//这个必须要填写，会在signalfd_siginfo中以ssi_ptr指针返回
    cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    cb.aio_sigevent.sigev_signo = SIGUSR1;
    //创建 signalfd
    sigset_t mask;
    // 信号清零
    sigemptyset(&mask);
    // 添加信号到掩码集
    sigaddset(&mask, SIGUSR1);
    // 设置该进程为对应的信号集的内容（当前已经的信号集合做并集、交集、覆盖）
    // 这行代码才是真正的信号设置；
    sigprocmask(SIG_BLOCK, &mask, NULL);

    // 创建 signalfd 句柄（绑定信号），用于epoll下可以考虑设置为非阻塞fd
    int sfd = signalfd(-1, &mask, SFD_NONBLOCK);

    printf("handle aio by signalfd \n");
    //创建epollfd
    const int MAX_EVENT_NUM = 1;
    int efd = epoll_create(MAX_EVENT_NUM);
    //添加sfd到epoll的监听中
    epoll_event in_ev{EPOLLIN, {.fd=sfd}};
    epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &in_ev);
    //阻塞，监听epoll返回
    epoll_event out_ev;
    int ret = epoll_wait(efd, &out_ev, MAX_EVENT_NUM, -1);
    if(ret <=0){
        printf(" epoll fail with ret: %d \n", ret);
        return;
    }
    //若out_ev里边有可写事件，尝试读取数据
    if(EPOLLIN & out_ev.events){
        signalfd_siginfo sig;
        size_t s = read(sfd, &sig, sizeof(struct signalfd_siginfo));
        // ...
        // 信号的逻辑处理
        if(s > 0){
            SignalFdHandler(sig.ssi_signo, (aiocb*)sig.ssi_ptr, nullptr);
        } else {
            printf("get signal size %d\n", s);
        }
    }
}

/**
 * @brief 异步IO：aio （ 即POSIX aio） glibc在用户态基于多线程实现的aio，不需要内核支持，编译时需要链接库 librt.so
 * 
 * @return std::string 
 */
std::string ReadByGlibcAIO(){

    const int SEQ_BUF_SIZE = 1024;
    char buf[SEQ_BUF_SIZE];
    bzero(buf, SEQ_BUF_SIZE);
    struct aiocb cb;
    memset(&cb, 0, sizeof(struct aiocb));
    int fd = open("./test.txt", O_RDONLY);

    //step1. 创建 posix aio 回调对象
    cb.aio_fildes = fd; 
    cb.aio_buf = buf;
    cb.aio_nbytes = SEQ_BUF_SIZE;
    cb.aio_offset = 0; // start offset

    //step2. 创建异步读取任务
    int ret = aio_read(&cb);
    if (ret != 0){
         fprintf(stderr, "aio_read() failed. errno = %d\n", errno);
         return "";
    }
    //step3. 通过sleep、信号、线程、signalfd+epoll等方式，监听异步任务结束并执行结果
    //CheckBySleep(cb);
    //CheckByThread(cb);
    //CheckBySignal(cb);
    //CheckBySignalFd(cb);
    CheckByEpoll(cb);
    sleep(3);
    return "";
}