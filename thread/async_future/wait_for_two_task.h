#ifndef __wait_for_two_task__
#define __wait_for_two_task__

#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <exception>
#include <future>

namespace wait_for_two_task
{
    int doSomething(char c)
    {

        std::default_random_engine dre(c);
        std::uniform_int_distribution<int> id(10, 1000);
        for (int i = 0; i < 5; i++)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(id(dre)));
            std::cout.put(c).flush();
        }
        return c;
    }
    void TestPassingArguments()
    {
        char a = 'a', b='b', c='c', d='d';
        //lamda值传递
        std::future<int> res1 = std::async(std::launch::async, [=]{return doSomething(a);});
        //函数值传递
        std::future<int> res2 = std::async(std::launch::async, doSomething, b);

        //lamda引用传递
        std::future<int> res3 = std::async(std::launch::async, [&]{return doSomething(c);});
        //函数引用传递
        std::future<int> res4 = std::async(std::launch::async, doSomething, std::ref(d));

        std::cout << "res1:" << res1.get() << "res2:" << res2.get()<< "res3:" << res3.get()<< "res4:" << res4.get() << std::endl;
    }
} // namespace wait_for_two_task

#endif