#ifndef __thread_promise__
#define __thread_promise__
#include <thread>
#include <chrono>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <random>
#include <future>

namespace thread_promise {
    int doSomething(char c){
        std::default_random_engine dre(c);
        std::uniform_int_distribution<int> id(10, 1000);
        for(int i=0;i<5;i++){
            std::this_thread::sleep_for(std::chrono::milliseconds(id(dre)));
            std::cout << " in thread " << std::this_thread::get_id() << " :" << c << std::endl;
        }
        return c;
    }

    void doSomethingPromise(std::promise<std::string> &p){
        try{
            std::srand(324312);
            int rand = std::rand() % 100;
            std::cout << "current thread:" << std::this_thread::get_id() << std::endl;
            if(rand>50){
                p.set_value("promise resolve!" + std::to_string(rand));
            } else {
                throw std::runtime_error("promise reject " + std::to_string(rand));
            }
        } catch(...){
            p.set_exception(std::current_exception());
        }
    }

    //Join，会等到线程中的异步任务执行结束
    //两个线程在创建后会立即同时开始异步任务，这俩符号交替打出
    void TestJoin(){
        std::cout << "main thread:" << std::this_thread::get_id() << std::endl;
        std::thread t1(doSomething, '+');
        std::thread t2(doSomething, '-');
        t1.join();
        t2.join();
    }

    //detach，程序不会管线程是否执行结束，直接结束
    void TestDetach(){
        std::thread t1(doSomething, '+');
        std::thread t2(doSomething, '-');
        t1.detach();
        t2.detach();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "job done!" << std::endl;
    }

    // Promise，可以resolve、reject
    // Promise不仅可以用在多线程场景中，也可以用来在单线程场景持有一个结果或者异常，放后面处理
    void TestPromise(){
        std::promise<std::string> p;
        // 注意这里传递promsie的引用进去，要用std::ref(p)，而不能 &p
        std::thread t(doSomethingPromise, std::ref(p));
        t.detach();
        std::future<std::string> f = p.get_future();
        try{
            std::cout << f.get() << std::endl;
        } catch(std::exception &e){
            std::cout << e.what() << std::endl;
        }
    }


    double compute(int a, int b){
        std::cout << " in thread " << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(a+b));
        return a+b;
    }
    
    //打包任务，用于在线程池中控制任务启动等操作
    void TestPackagedTask(){
        std::packaged_task<double(int, int)> pt(compute), pt2(compute);
        std::future<double> f = pt.get_future(), f2 = pt2.get_future();
        std::cout << " start packaged task " << std::endl;
        //打包好的任务一般会放到别处，例如线程池中去执行，这里可以利用thread或者异步launch的future模拟一下
        //多个打包好的任务，会并行运行
        std::thread t1([&]{pt(1, 4);});
        std::thread t2([&]{pt2(1, 4);});
        t1.detach();
        t2.detach();
        // std::future<void> _f1 = std::async(std::launch::async, [&]{pt(1, 4);});
        // std::future<void> _f2 = std::async(std::launch::async, [&]{pt2(1, 4);});
        // _f1.get();
        // _f2.get();
        std::cout << " packaged task result: " <<  f.get() << " result2:" << f2.get() << std::endl;
    }

}

#endif