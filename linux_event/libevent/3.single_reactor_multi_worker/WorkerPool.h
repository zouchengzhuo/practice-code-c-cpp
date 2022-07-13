/*
 * @Date: 2022-07-12 14:49:58
 * @LastEditors: czzou
 * @LastEditTime: 2022-07-13 06:27:54
 * @FilePath: /practice-code-c-cpp/linux_event/libevent/3.single_reactor_multi_worker/WorkerPool.h
 */
#ifndef __workerpool_h__
#define __workerpool_h__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sstream>
#include <strings.h>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

/**
 * @description: 一个任务线程池，启动N个线程，不断尝试从队列中取出任务执行
 * @return {*}
 */
class WorkerPool
{
public:
    typedef std::function<void()> Runable;
    WorkerPool(){};
    WorkerPool(size_t size) : pool_size(size){};
    void start()
    {
        for(int i=0;i<this->pool_size;i++){
            std::thread t(&WorkerPool::run_worker, this);
            t.detach();
        }
    }
    void addQueue(Runable task)
    {
        std::lock_guard<std::mutex> l(this->lock);
        this->task_queue.push(task);
        this->conditiob_var.notify_one();
    }

private:
    std::mutex lock;
    std::condition_variable conditiob_var;
    size_t pool_size;
    std::queue<Runable> task_queue;

private:
    void run_worker()
    {
        Runable task = nullptr;
        while (1)
        {
            task = nullptr;
            {
                std::unique_lock<std::mutex> l(this->lock);
                this->conditiob_var.wait(l, [this]
                                         {
                        //wait一般情况下只会被 condition_variable的 notify_one、 notify_all 唤醒，但是也存在一定可能性误唤醒
                        //所以这里再加一道check
                        return !this->task_queue.empty(); });
                printf("queue size before: %u\n", this->task_queue.size());
                task = this->task_queue.front();
                this->task_queue.pop();
                printf("queue size after: %d\n", this->task_queue.size());
            }
            if (task == nullptr)
                continue;
            task();
        }
    }
};

#endif