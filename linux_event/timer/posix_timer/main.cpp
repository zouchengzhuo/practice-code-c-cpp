#include <unistd.h>
#include <iostream>
#include <signal.h>           /* Definition of SIGEV_* constants */
#include <time.h>

//通过信号 handler 触发
void  signal_function_timer(int signalNum)
{
    printf("get signal num: %u\n", signalNum);
}

//通过线程触发
void thread_function_timer(sigval_t t){
    printf("get signal num by thread: %u\n", t.sival_int);
}

//设置为信号触发
void setup_evp_by_signal(sigevent &evp, timer_t& timer){
    evp.sigev_value.sival_ptr = &timer;
    //通过信号的方式触发，还可以通过线程的方式触发 SIGEV_THREAD、SIGEV_THREAD_ID
    evp.sigev_notify = SIGEV_SIGNAL; 
    evp.sigev_signo = SIGUSR1;
    //为信号绑定处理函数
    signal(SIGUSR1, signal_function_timer);
}

//设置为线程触发，注意编译时需要链接上 -lpthread
void setup_evp_by_thread(sigevent &evp, timer_t& timer){
    evp.sigev_value.sival_ptr = &timer;
    evp.sigev_value.sival_int = SIGUSR1;
    evp.sigev_notify = SIGEV_THREAD; 
    evp.sigev_notify_function = thread_function_timer;
    //可以通过sigev_notify_attributes设置新建线程的参数，此结构体创建方式见：
    //https://man7.org/linux/man-pages/man3/pthread_attr_init.3.html
    //注意，这里即使不用，也要设置为一个空指针，否则不能触发
    evp.sigev_notify_attributes = nullptr;

}

int main(){
    //step1.定义posix定时器指针变量
    timer_t timer;

    //step2.创建 signal event
    struct sigevent evp;
    //通过signal触发
    //setup_evp_by_signal(evp, timer);
    setup_evp_by_thread(evp, timer);

    //step3.以操作系统启动时间创建一个定时器
    int ret = timer_create(CLOCK_MONOTONIC, &evp, &timer);
    if( ret ) perror("timer_create error");

    //step4.获取系统启动时间的高精度时间值
    struct timespec spec;
    clock_gettime(CLOCK_MONOTONIC, &spec);  
    //定时器相关时间配置
    struct itimerspec time_value;
    //每3秒触发一次
    time_value.it_interval.tv_sec = 3; 
    time_value.it_interval.tv_nsec = 0;
    //3秒后启动定时器
    time_value.it_value.tv_sec = spec.tv_sec + 3;
    time_value.it_value.tv_nsec = spec.tv_nsec + 0;
    //step5.启动定时器
    ret = timer_settime(timer, CLOCK_MONOTONIC, &time_value, NULL);
    if( ret ) perror("timer_settime error");
    while(1)
    {
        //读取一下还剩余多长时间触发定时器
        itimerspec its;
        ret = timer_gettime(timer, &its);
        if( ret ) perror("timer_gettime error");
        printf("get itimerspec left seconds: %u, it_interval: %u \n", its.it_value.tv_sec, its.it_interval.tv_sec);
        //如果有之前的信号未处理，当前信号触发后会丢弃，称为 over run，这里会返回数字
        int overRunNum = timer_getoverrun(timer);
        printf("overRunNum: %u\n", overRunNum);
        usleep(500*1000);
    }
}