#include <iostream>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <unistd.h>

void ReadBlock(){
    //step1.以操作系统启动时间创建一个定时器fd
    int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (tfd == -1) perror("timerfd_create");
    //step2.为timerfd设置超时信息
    itimerspec time_value;
    //每3秒触发一次
    time_value.it_interval.tv_sec = 3;
    time_value.it_interval.tv_nsec = 0;
    //设置启动时间，3s后启动
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    time_value.it_value.tv_sec = t.tv_sec + 3;
    time_value.it_value.tv_nsec = t.tv_nsec;
    int ret = timerfd_settime(tfd, TFD_TIMER_ABSTIME, &time_value, NULL);
    if(ret == -1) perror("timerfd_settime");
    //step3.read读取超时次数，如果未超时则进入阻塞状态
    uint64_t count;
    while (true)
    {
        ret = read(tfd, &count, sizeof(count));
        if(ret == -1){
            perror("read timerfd");
            break;
        }
        printf("read timeout count: %u\n", count);
    }
}

void ReadNonblockByEpoll(){
    //step1.以操作系统启动时间创建一个定时器fd，非阻塞模式
    int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (tfd == -1) perror("timerfd_create");
    //step2.为timerfd设置超时信息
    itimerspec time_value;
    //每3秒触发一次
    time_value.it_interval.tv_sec = 3;
    time_value.it_interval.tv_nsec = 0;
    //设置启动时间，3s后启动
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    time_value.it_value.tv_sec = t.tv_sec + 3;
    time_value.it_value.tv_nsec = t.tv_nsec;
    int ret = timerfd_settime(tfd, TFD_TIMER_ABSTIME, &time_value, NULL);
    if(ret == -1) perror("timerfd_settime");
    //step3.通过 epoll 监听 timerfd
    uint64_t count;
    int efd = epoll_create(10);
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = tfd;
    ret = epoll_ctl(efd, EPOLL_CTL_ADD, tfd, &ev);
    if(ret ==-1) perror("epoll_ctl add");
    epoll_event evs[1];
    while (true)
    {
        ret = epoll_wait(efd, evs, 1, -1);
        if(ret == -1) perror("epoll_wait");
        if(ret == 0) continue;
        ret = read(tfd, &count, sizeof(count));
        printf("epoll get count: %u\n", count);
    }
}

int main(){
    //ReadBlock();
    ReadNonblockByEpoll();
}