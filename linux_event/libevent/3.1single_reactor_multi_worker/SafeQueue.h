/*
 * @Date: 2022-07-13 09:30:17
 * @LastEditors: czzou
 * @LastEditTime: 2022-07-13 15:24:50
 * @FilePath: /practice-code-c-cpp/linux_event/libevent/3.1single_reactor_multi_worker/SafeQueue.h
 */
#ifndef __SAFE_QUEUE_H__
#define __SAFE_QUEUE_H__

#include <queue>
#include <mutex>
#include <condition_variable>

// A threadsafe-queue.
template <class T>
class SafeQueue
{
public:
  SafeQueue(void)
    : q()
    , m()
    , c()
  {}

  ~SafeQueue(void)
  {}

  void push(T t)
  {
    std::lock_guard<std::mutex> lock(m);
    q.push(t);
    c.notify_one();
  }

  //获取并删除第一个元素，若无元素，则阻塞当前线程等待
  T shift(void)
  {
    std::unique_lock<std::mutex> lock(m);
    while(q.empty())
    {
      c.wait(lock);
    }
    T val = q.front();
    q.pop();
    return val;
  }

  bool shift_no_wait(T &val)
  {
    std::lock_guard<std::mutex> lock(m);
    if(q.empty()) return false;
    val = q.front();
    q.pop();
    return true;
  }

  bool empty(){
    std::lock_guard<std::mutex> lock(m);
    return q.empty();
  }

  int size(){
    std::lock_guard<std::mutex> lock(m);
    return q.size();
  }

private:
  std::queue<T> q;
  mutable std::mutex m;
  std::condition_variable c;
};
#endif