参考文章/文档：  
https://arthurchiao.art/blog/intro-to-io-uring-zh/  io_uring 好文   
https://zhuanlan.zhihu.com/p/62682475  io_uring 好文     
https://juejin.cn/post/7074212680071905311 io_uring和epoll的性能对比

可以自行安装liburing 和 liburing-devel 来开发程序，这两个库封装了 io_uring 的API，更加易用。  

yum install -y liburing
yum install https://vault.centos.org/centos/8/PowerTools/aarch64/os/Packages/liburing-devel-1.0.7-3.el8.aarch64.rpm （需要选择对应环境的架构版本）

TODO：  
submit queue、complete queue、mmap，以及三种wait模式的原理理解。