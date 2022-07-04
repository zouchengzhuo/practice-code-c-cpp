#ifndef __condition_variable_test__
#define __condition_variable_test__
#include <thread>
#include <chrono>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <random>
#include <future>
#include <condition_variable>
#include <queue>
#include <atomic>

class SpinLock {

public:
    SpinLock() : flag_(false)
    {}

    void lock()
    {
        bool expect = false;
        while (!flag_.compare_exchange_weak(expect, true))
        {
            //这里一定要将expect复原，执行失败时expect结果是未定的
            expect = false;
        }
    }

    void unlock()
    {
        flag_.store(false);
    }

private:
    std::atomic<bool> flag_;
};

//#define LOCK_TYPE std::mutex
#define LOCK_TYPE SpinLock

namespace condition_variable_test{


    bool readyFlag = false;
    LOCK_TYPE readyFlagMutex;
    //std::condition_variable readyCondVar;
    std::condition_variable_any readyCondVar;
    
    //sleep一段时间，将readyFlag设置为true，然后触发一次notify_one
    void prepareThread(){
        std::this_thread::sleep_for(std::chrono::seconds(5));
        {
            std::lock_guard<LOCK_TYPE> lg(readyFlagMutex);
            std::cout << " 1. prepare get lock" << std::endl; 
            readyFlag = true;
            readyCondVar.notify_one();
            std::cout << " 2. prepare call notify and sleep " << std::endl; 
            //std::this_thread::sleep_for(std::chrono::seconds(5));
        }
        
    }
    
    //通过条件变量的wait让线程进入休眠状态，通过一个unique_lock，来实现收到notify之后执行的 锁定-检查-解锁逻辑
    void waitForPrepareThread(){
        while (true)
        {
            {
            //这里一定要使用 unique_lock，不能使用lock_guard
            std::unique_lock<LOCK_TYPE> ul(readyFlagMutex);
            //两种wait：
            //第一种，只传入一个 unique_lock，此时会执行 ① 获取锁 ②阻塞线程，进入休眠状态 ③ 释放锁 ④直到被一个notify触发，线程被唤醒，执行wait之后的逻辑
            readyCondVar.wait(ul);
            //第二种：为了处理假唤醒，多传入一个判断函数，加锁后，判断函数返回false的时候才释放锁并执行后面的逻辑，否则释放锁并进入休眠状态
            // readyCondVar.wait(ul, []{
            //     //wait一般情况下只会被 condition_variable的 notify_one、 notify_all 唤醒，但是也存在一定可能性误唤醒
            //     //这里不传入lamda做判断的话，误唤醒时实际上readyFlag并没有变成true
            //     //所以这里再加一道check
            //     std::cout << " condition_variable notified, check readyFlag... " << std::endl;
            //     return readyFlag;
            // });
            std::cout << " 3. condition_variable check pass... " << std::endl;
            }
            std::cout << " 4. prepare job done, run something! " << std::endl; 
        }
        
        
    }

    void testConditionVariable(){
        std::thread t1(prepareThread);
        std::thread t2(waitForPrepareThread);
        t1.join();
        t2.join();
    };

    std::queue<int> queue;
    void provider(int val){
        std::this_thread::sleep_for(std::chrono::seconds(5));
        for(int i=0;i<50;i++)
        {
            {
                std::lock_guard<LOCK_TYPE> lg(readyFlagMutex);
                queue.push(val+i);
            }
            //用notify_one，只会唤醒一个 condition_variable.wait
            readyCondVar.notify_one();
        }
        //用notify_all，会唤醒全部的 condition_variable.wait。 
        //readyCondVar.notify_all();
    }

    void consumer(std::string name){
        int val;
        while (true)
        {
            {
                std::unique_lock<LOCK_TYPE> ul(readyFlagMutex);
                //readyCondVar.wait(ul); //如果用 wait(ul)+notify_*，那么可能有几个 consunmer线程，任务就只会被执行几个
                //wait(ul, predicate)可以清空队列
                readyCondVar.wait(ul, [&]{
                    return !queue.empty();
                });
                std::cout << name << " notified, queue size: " << queue.size() << std::endl;
                //对queue的操作需要在ul的保护范围内进行，以免造成线程安全未知错误
                val = queue.front();
                queue.pop();
                //ul生命周期结束，锁自动释放
            }
            std::cout << name << " notified, get value: " << val << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    };

    void testQueue(){
        std::thread pro1(provider, 1);
        std::thread consum1(consumer, "tom");
        std::thread consum2(consumer, "lily");
        std::thread consum3(consumer, "lihua");
        std::thread consum4(consumer, "cat");
        std::thread consum5(consumer, "dog");
        pro1.join();
        consum1.join();
        consum2.join();
        consum3.join();
        consum4.join();
        consum5.join();
    };


};

#endif