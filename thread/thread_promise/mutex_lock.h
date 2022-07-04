#ifndef __mutex_lock__
#define __mutex_lock__
#include <thread>
#include <chrono>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <random>
#include <future>
namespace mutex_lock{

    std::mutex mutex;
    std::recursive_mutex r_mutex;
    std::timed_mutex t_mutex;

    void run(const std::string msg) {
        int i = 5;
        while (i>0)
        {
            i--;
            std::cout << "thread id:" << std::this_thread::get_id() << " msg:" << msg << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 1000));
        }
    }

    void runWithLock(const std::string msg){
        mutex.lock();
        run(msg);
        mutex.unlock();
    }

    void runWithTimedLock(const std::string msg){
        t_mutex.lock();
        std::cout << "lock t_mutex" << std::endl;
        run(msg);
        t_mutex.unlock();
        std::cout << "unlock t_mutex" << std::endl;
    }

    void runWithLockGuard(const std::string msg){
        //lock_guard可以在局部作用于内自动管理对mutex的lock，结束后自动释放
        run(msg);
        {
            std::lock_guard<std::mutex> lg(mutex);
            run(msg);
        }
    }

    void runWithRecursiveMutex(){
        std::lock_guard<std::recursive_mutex> lg(r_mutex);
        std::cout << "runWithRecursiveMutex" << std::endl;
    }

    //测试不用mutex和用mutex，一个交替输出，一个顺序输出
    void TestMutex(){
        
        std::thread t1(run, "test mutex1");
        std::thread t2(run, "test mutex2");
        t1.join();
        t2.join();
        std::cout << "no mutex job done!" << std::endl;
        std::thread t3(runWithLock, "test mutex3");
        std::thread t4(runWithLock, "test mutex4");
        t3.join();
        t4.join();
        std::cout << " mutex job done!" << std::endl;
    }

// std::lock_guard，自动管理锁的生命周期，在析构时自动unlock
void TestLockGuard(){
    std::thread t1(runWithLockGuard, "test mutex1");
    std::thread t2(runWithLockGuard, "test mutex2");
    t1.join();
    t2.join();
    std::cout << " lock_guard job done!" << std::endl;
}

    //recursive_mutex 递归锁，解决递归死锁问题
    void TestRecursiveMutex(){

        // 这里存在对mutex的递归lock，当runWithLockGuard运行到需要获取锁的地方时，会形成死锁
        // std::lock_guard<std::mutex> lg(mutex);
        // std::cout << "test no recursive mutex" << std::endl;
        // runWithLockGuard("test mutex");


        //rescursive_mutex，允许  !同一线程！ 对rescursive_mutex多次lock，在最后一次的unlcok处释放
        // 跨线程还是会死锁的
        std::lock_guard<std::recursive_mutex> lg(r_mutex);
        std::cout << "test recursive mutex" << std::endl;
        runWithRecursiveMutex();
        // 这里即使用rescursive_mutex，也会形成死锁，因为跨线程了
        // std::thread t1(runWithRecursiveMutex);
        // t1.join();
        std::cout << "job done!" << std::endl;
    }

    //try_lock 和 lock_for，控制获取锁失败的情况
    void TestTryLockAndLockFor(){
        //加锁失败放弃执行
        {
            mutex.lock();
            if(mutex.try_lock() == true){
                run("try_lock success");
            } else {
                std::cout << "try_lock fail" << std::endl;
            }
            mutex.unlock();
        }
        
        //加锁成功，用std::adopt_lock使用lock_guard
        {
            if(mutex.try_lock() == true){
                //这里因为在try_lock的时候已经lock了，所以用lock_guard要传入一个adopt_lock
                std::lock_guard<std::mutex>(mutex, std::adopt_lock);
                run("try_lock success");
                //std::cout << "try_lock success" << std::endl;
            } else {
                std::cout << "try_lock fail" << std::endl;
            }
        }

        // try_lock_for/try_lock_until等待一定时间，需要使用 timed_mutex或者 rescursive_timed_mutex
        // 这里要注意的是 gcc4.8下运行此代码， t_mutex.try_lock_for总是会立即返回失败，不符合预期中的等待30s
        // 据说高版本gcc上才能比较好的支持c++11的特性 https://bbs.csdn.net/topics/397728258
        {
            std::thread t(runWithTimedLock, "lock t_mutex");
            //sleep 1s，确保lock线程先起来，好让waitfor真的进入等待状态
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::thread t2([]{
                if(t_mutex.try_lock_for(std::chrono::seconds(30)) == true){
                    std::lock_guard<std::timed_mutex> lg(t_mutex, std::adopt_lock);
                    run("try_lock_for success");
                } else {
                    std::cout << "try_lock_for fail" << std::endl;
                }
            });
            t.join();
            t2.join();
        }
    }
    std::mutex m1, m2;
    // 同时锁定多个mutex
    void TestMultiMutex(){
        //上面的lock、try_lock，都能应用于多个mutex，只有都成功时才一起lock所有的mutex。其中try_lock在都lock成功时返回-1，若不成功，则哪个不成功就返回其索引
        std::lock(m1, m2);
        std::lock_guard<std::mutex>(m1, std::adopt_lock);
        std::lock_guard<std::mutex>(m2, std::adopt_lock);
        //Do Something
        //使用try_lock的话，会返回第一个获取锁失败的index，如果全部成功，会返回-1
        int failIndex = std::try_lock(m1, m2);
        if(failIndex < 0){
            std::lock_guard<std::mutex>(m1, std::adopt_lock);
            std::lock_guard<std::mutex>(m2, std::adopt_lock);
        } else {
            // failIndex 就是第一个失败的mutex的索引
            // 这里不需要手动释放锁定成功的mutex，因为一旦有锁定失败的，之前锁定成功的也会自动释放
        }
    }

    std::mutex readyFlagMutex;
    bool readyFlag;
    void prepareThread(){
        //做某种准备工作，花费5秒钟
        std::this_thread::sleep_for(std::chrono::seconds(5));
        //加锁然后将readyFlag设置为true，代表已经准备好了
        std::lock_guard<std::mutex> lg(readyFlagMutex);
        readyFlag = true;
        //设置完毕后，lock_guard生命周期结束，自动释放锁
    }

    void waitForPrepareThread(){
        {
            //在此作用域内，声明一个unique_lock，阻塞并尝试获取锁
            std::unique_lock<std::mutex> ul(readyFlagMutex);
            //死循环等待readyFlag变为true
            while (!readyFlag)
            {
                //如果还没有变为true，就sleep 500ms，把cpu让出来让程序去做准备工作或者其它事情
                //这里注意在等待之前，一定要把锁释放掉，不然prepare thread可能因为此处占着锁而没办法修改readyFlag，造成死锁
                ul.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::cout << " wait for prepare... " << std::endl;
                //sleep完成后，再次加锁判断，直到readyFlag变为true
                ul.lock();
            }
            //注意：当readyFlag变为true时，ul到这里还是拥有锁的状态，随着它生命周期结束，锁会被自动释放
        }
        std::cout << " prepare job done, run something! " << std::endl;
    };

    void TestUniqueLock(){
        std::thread t1(prepareThread);
        std::thread t2(waitForPrepareThread);
        t1.join();
        t2.join();
    }

}

#endif