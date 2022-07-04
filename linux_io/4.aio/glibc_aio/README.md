参考文章/文档：  
https://gohalo.me/post/linux-program-aio.html  aio介绍
https://man7.org/linux/man-pages/man7/aio.7.html aio文档
https://zhuanlan.zhihu.com/p/371574406 IO异步同步类型
https://www.jianshu.com/p/686e2299a017 FILE文件缓冲区

Linux 中有两套异步 IO  
- 一套是由 glibc 实现的 aio_* 系列，通过线程+阻塞调用在用户空间模拟 AIO 的功能，不需要内核的支持，类似的还有 libeio；
- 另一套是采用原生的 Linux AIO，并由 libaio 来封装调用接口，相比来说更底层。