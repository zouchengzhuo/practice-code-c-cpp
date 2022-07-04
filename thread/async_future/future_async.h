#ifndef __future_async_h__
#define __future_async_h__

#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <exception>
#include <future>

namespace future_async
{
    struct CustomException : public std::exception
    {
        const char *what() const throw()
        {
            return "Custom Exception";
        }
    };

    int doSomething(char c)
    {

        std::default_random_engine dre(c);
        std::uniform_int_distribution<int> id(10, 1000);
        for (int i = 0; i < 5; i++)
        {
            auto sleep_time = std::chrono::milliseconds(id(dre));
            std::this_thread::sleep_for(sleep_time);
            std::cout << "thread:" << std::this_thread::get_id() << std::endl;
            std::cout.put(c).flush();
        }
        return c;
    }

    int func1()
    {
        return doSomething('.');
    }

    int func2()
    {
        return doSomething('+');
    }

    void func_ex()
    {
        doSomething('=');
        throw CustomException();
    }

    // 测试普通async异步
    // 默认由系统调度合适开始异步任务，任何时间都有可能开启;当调用 .get()时，强制开启线程任务
    // 此时会先阻塞执行完func2，再阻塞执行完func1，实际上是没有异步操作的
    void TestAsync()
    {
        std::cout << "main thread:" << std::this_thread::get_id() << std::endl;
        std::cout << "start func1 in background and func2 foreground" << std::endl;
        std::future<int> result1(std::async(func1));
        int result2 = func2();
        int result = result1.get() + result2;
        std::cout << " final result :" << result << std::endl;
    }

    void TestSharedAsync()
    {
        std::cout << "main thread:" << std::this_thread::get_id() << std::endl;
        std::cout << "start func1 in background and func2 foreground" << std::endl;
        //std::future<int> result1(std::async(func1));
        std::shared_future<int> result1(std::async(func1));
        int result2 = func2();
        int result = result1.get() + result1.get() + result2;
        std::cout << " final result :" << result << std::endl;
    }


    //测试两个异步。因为都是用.get的方式强制启动的，实际上整个过程是串行的
    void TestTwoAsync()
    {
        //这种方式虽然两个都是异步的，但是其实会顺序执行
        std::cout << "start func1 in background and func2 background" << std::endl;
        std::future<int> result1(std::async(func1));
        std::future<int> result2(std::async(func2));
        //int result2 = func2();
        //std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        int result = result1.get() + result2.get();
        std::cerr << " final result :" << result << std::endl;
    }

    //测试通过 async launch
    //如果传入 std::launch::async，则会立即开始异步任务，不行则抛异常。此时多个任务是真正的并行执行的
    void TestLaunchAsync()
    {
        //launch async可以强制直接异步启动，可看到两个符号交替打出
        std::cout << "start func1 in background and func2 background, launch by async" << std::endl;
        // 注意，这里如果不支持异步，会抛出一个system error，例如编译的时候没有连接 libpthread.a
        std::future<int> result1(std::async(std::launch::async, func1));
        std::future<int> result2(std::async(std::launch::async, func2));
        //int result2 = func2();
        //注意，当用launch async的时候，这里即使不get，生命周期结束时，异步函数也会被执行
        std::cout << " before final " << std::endl;
        int result = result1.get() + result2.get();
        std::cout << " final result :" << result << std::endl;
    }

    // 测试通过deferred launch
    // 当用launch deferred的时候，调用get之前绝对不会执行异步任务
    // 因为最终还是调用get时启动且阻塞当前线程的，所以这俩任务实际上也是串行，而不是异步并行的。
    void TestLaunchDeferred()
    {
        std::cout << "start func1 in background and func2 background, launch by deferred" << std::endl;
        std::future<int> result1(std::async(std::launch::deferred, func1));
        std::future<int> result2(std::async(std::launch::deferred, func2));
        //int result2 = func2();
        //std::default_random_engine dre('x');
        std::default_random_engine dre('x');
        std::uniform_int_distribution<int> id(10, 1000);
        int random = id(dre);
        while (random < 500)
        {
            std::cout << " not to call get, and not start deferred task, random:" << random << std::endl;
            random = id(dre);
        }
        std::cout << " random: " << random << std::endl;
        int result = result1.get() + result2.get();
        std::cout << " call get and start deferred task, final result :" << result << std::endl;
    }

    //测试异常处理，像普通的异常一样catch，但是这个异常是在调用get后，才能得到并抛出的，就像get一个普通的结果一样。
    void TestHandleException()
    {
        try
        {
            std::future<void> ex(std::async(func_ex));
            ex.get();
        }
        catch (std::exception &e)
        {
            std::cout << "e:" << e.what() << std::endl;
        }
    }

    // 测试wait, wait会强制启动一个async任务，wait时会阻塞当前线程，所以这两个wait的任务是同步串行的
    void TestWait()
    {
        std::future<int> result1 = std::async(func1);
        std::future<int> result2 = std::async(func2);
        result1.wait();
        result2.wait();
        std::cout << " async task done " << std::endl;
        std::cout << " wait会强制启动async任务：" << (result1.get()+result2.get()) << std::endl;
    }

    // 测试wait_for，阻塞当前线程，等待固定的时间，不会强制启动任务
    // wait_until也一样，只不过可以等待到某具体时间点
    // 等待超时后，当前线程会继续执行不再等待，而future线程中的任务并不会终止，还是会继续执行完
    std::future<int> f;
    void TestWaitFor()
    {
        int result = 1;
        //不加launch，因为wait_for不会主动开启任务，这里任务都不会开始执行，怎么等都是timeout
        //std::future<int> f = std::async(func1);

        //加launch::async，这里根据等待时间不同，可能是ready或者timeout，但是有个问题，无论是哪种情况，函数析构的时候都会等待async的任务执行完毕
        //std::future<int> f = std::async(std::launch::async, func1);

        //这种写法，把future对象放外面，在此函数结束的时候，不会等待f执行结束。 不过此demo中，在程序退出前仍然会等待退出
        //f = std::async(std::launch::async, func1);

        //通过deferred，wait_for之后也总是timeout
        //TODO: std::future_status::deferred如何触发？
        f = std::async(std::launch::deferred, func1);

        //等一段时间，若异步的有结果了则返回异步的，否则返回别的
        std::future_status status = f.wait_for(std::chrono::milliseconds(500));
        switch (status)
        {
        case std::future_status::deferred:
            std::cout << "还未开启异步任务" << std::endl;
            break;
        case std::future_status::ready:
            std::cout << "异步任务已完成" << std::endl;
            break;
        case std::future_status::timeout:
            std::cout << "异步任务等待时间到了，还没结束" << std::endl;
            break;
        default:
            break;
        }
    }

    // 测试利用等待0时间来轮询
    void TestPolling()
    {
        //这样会一直卡住，c++标准库中对不带launch选项的async的解释，是会自动在合适的时机拉起，但是这里gcc4.8中，看起来永远不会开启任务，一直卡在循环中
        // std::future<int> result = std::async(func1);
        // while(result.wait_for(std::chrono::seconds(0)) != std::future_status::ready){
        //     std::cout << "wait for 0, polling..." << (result.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) << std::endl;
        //     std::this_thread::sleep_for(std::chrono::seconds(1));
        // }
        // std::cout << "wait for polling done:" << result.get() << std::endl;

        //这样也会一直卡住，或许是运行环境的原因，这里和《C++标准库》里的描述不同。实际运行时，这个wait_for(0)一直会返回timeout
        //可能在某些编译器或者运行环境下，不带launch的async是会立即启动的
        // std::future<int> result = std::async(func1);
        // if(result.wait_for(std::chrono::seconds(0)) != std::future_status::deferred){
        //     while(result.wait_for(std::chrono::seconds(1)) != std::future_status::ready){
        //         std::cout << "wait for 0, polling..." << std::endl;
        //         std::this_thread::sleep_for(std::chrono::seconds(1));
        //     }
        // }

        // 貌似只有这样，主动用async 来launch，才能polling成功
        std::future<int> result = std::async(std::launch::async, func1);
        if (result.wait_for(std::chrono::seconds(0)) != std::future_status::deferred)
        {
            while (result.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready)
            {
                std::cout << "wait for 100ms, polling..." << std::endl;
                std::this_thread::yield();
            }
        }
        std::cout << "wait for polling done:" << result.get() << std::endl;
    }

} // namespace future_async

#endif