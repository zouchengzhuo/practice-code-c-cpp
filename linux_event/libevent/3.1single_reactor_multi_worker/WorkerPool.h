/*
 * @Date: 2022-07-12 14:49:58
 * @LastEditors: czzou
 * @LastEditTime: 2022-07-13 19:03:13
 * @FilePath: /practice-code-c-cpp/linux_event/libevent/3.1single_reactor_multi_worker/WorkerPool.h
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
#include <functional>
#include "SafeQueue.h"

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
        this->task_queue.push(task);
    }

private:
    size_t pool_size;
    SafeQueue<Runable> task_queue;

private:
    void run_worker()
    {
        Runable task = nullptr;
        while (1)
        {
            task = nullptr;
            task = this->task_queue.shift();
            if (task == nullptr)
                continue;
            task();
        }
    }
};

#endif