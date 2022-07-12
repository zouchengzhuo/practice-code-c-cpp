单reactor单worker线程只是一种写法上的改进，`epoll_data_t` 这个 union 结构体  
```c++
typedef union epoll_data
{
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} epoll_data_t;
```
可以携带多种数据，自定义一个包含回调函数的event结构体，写入 ptr 指针，在有事件响应时利用回调函数处理 fd 上的事件，即为单reactor单worker模式