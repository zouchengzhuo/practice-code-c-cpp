zero copy分类及方式。  
参考文章： https://strikefreedom.top/linux-io-and-zero-copy  

- 减少或者避免内核态与用户态之间的数据拷贝
  - mmap
  - sendfile
  - sendﬁle() with DMA Scatter/Gather Copy
  - splice()
  - send() with MSG_ZEROCOPY
- 绕过内核的直接IO
  - 用户直接访问硬件
  - 内核控制访问硬件
- 内核缓冲区和用户缓冲区之间的传输优化
  - 动态重映射与写时拷贝 (Copy-on-Write)
  - 缓冲区共享 (Buffer Sharing)


