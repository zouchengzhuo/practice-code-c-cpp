#include <iostream>
#include <thread>
#include <chrono>

void fn1(){
    std::cout <<" in child thread:" << std::this_thread::get_id() << std::endl;
};
void timerFn1(std::string name){
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout <<" in timer child thread " << name << ":" << std::this_thread::get_id() << std::endl;
    timerFn1(name);
};

//thread.join: 阻塞调用join的线程，直到调用join的线程全部退出
void testJoin(){
    std::cout <<" in main thread:" << std::this_thread::get_id() << std::endl;
    std::thread t1(fn1);
    std::thread t2(timerFn1, "test");
    t1.join();
    t2.join();
    //由于t2线程会不断执行下去，这一行永远不会打出
    std::cout <<" in main thread done:" << std::this_thread::get_id() << std::endl;
};

//thread.detach: 线程脱离当前线程的控制，自己在后台执行下去，不阻塞当前线程。执行完后由运行容器进行回收，或者进程退出时回收。
void testDetach(){
    std::thread t1(timerFn1, "detach");
    std::thread t2(timerFn1, "join");
    t1.detach();
    std::cout <<" in main thread:" << std::this_thread::get_id() << std::endl;
    t2.join();
};
int main(){
    //testJoin();
    testDetach();
    return 0;
}