poll的作用与用法基本上和select是不一样的。  
区别是select用 `readfds, writefds, exceptfds` 这三个 fd_set 的bit位来表示fd关注的可读、可写、异常事件，数量有上限（如1024个）；  
poll用   
```c
struct pollfd {
    int   fd;         /* file descriptor */
    short events;     /* requested events */
    short revents;    /* returned events */
};
```
来表示监听的fd，传入一个 pollfd 结构体数组来表示poll监视的fd，数量是没有上限的。
