epoll与select和poll相比更加高效，主要区别体现在：  
- fd是通过 epoll_ctl 一次性添加到内核态或者从内核态删除的，相比之下select和poll每次都需要从用户态到内核态来一遍
- 用户态代码中无须对所有被监视的fd进行遍历，epoll直接返回了 ready list，取用就好了
- 支持LT模式和ET模式，其中ET模式只监听状态的变化，不持续触发，比较高效。不过ET模式只支持非阻塞socket，不然读完数据时会导致阻塞

参考文档：  
https://zhuanlan.zhihu.com/p/63179839 比较浅显易懂  

socketfd、signalfd、eventfd，都可以用 epoll 来监听；  
但是普通的ext4文件系统的文件不能用 epoll 来监听，因为没有提供poll接口，参考 https://cloud.tencent.com/developer/article/1835294