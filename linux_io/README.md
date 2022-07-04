# 同步&异步、阻塞&非阻塞

在描述同步&异步、阻塞&非阻塞时，一定要明确主语，否则描述无意义。  
经过一段时间的学习，我理解的主语有以下几种：文件、IO操作、多文件IO模型。  
其中 阻塞&非阻塞，一般用于描述文件；同步&异步则可以用来描述IO操作和多文件IO模型。  

## 阻塞&非阻塞与文件
可以通过 socket、accept等拿到网络通信fd，或者通过 open 拿到本地文件的fd。  
这些fd可以通过 fcntl 来设置为 阻塞&非阻塞，以决定在对他们执行IO操作时，是否阻塞进程。

## 同步&异步与IO操作
同步IO比如 read/recv/write 等，再从内核buffer到用户buffer之间互相拷贝数据时，都是同步的过程。  
异步IO比如 libaio、posixaio，TODO

# Zero-Copy


最新划分：
- fd
 - file
 - socket
- io_multi
 - select
 - poll
 - epoll
- aio
- zero-copy
