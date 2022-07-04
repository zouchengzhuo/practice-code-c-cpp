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


namespace atomic_test{


    std::atomic<bool> readyFlag(false);
    void prepareThread(){
        //做某种准备工作，花费5秒钟
        std::this_thread::sleep_for(std::chrono::seconds(5));
        readyFlag.store(true);
    }

    void waitForPrepareThread(){
        {
            //死循环等待readyFlag变为true
            while (!readyFlag.load())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::cout << " wait for prepare... " << std::endl;
            }
        }
        std::cout << " prepare job done, run something! " << std::endl;
    };

    void TestAtomic(){
        std::thread t1(prepareThread);
        std::thread t2(waitForPrepareThread);
        t1.join();
        t2.join();
    }
};

#endif