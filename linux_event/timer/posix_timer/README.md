linux内核提供了基于posix标准实现的定时器，主要涉及到的函数有：  

- [timer_create](https://man7.org/linux/man-pages/man2/timer_create.2.html) 创建定时器
- [timer_delete](https://man7.org/linux/man-pages/man2/timer_delete.2.html) 删除定时器
- [timer_settime](https://man7.org/linux/man-pages/man2/timer_settime.2.html) 启动/停止/重置 定时器
- [timer_gettime](https://man7.org/linux/man-pages/man2/timer_gettime.2.html) 获得定时器的到期时间和间隔
- [timer_getoverrun](https://man7.org/linux/man-pages/man2/timer_getoverrun.2.html) 获取超限次数（上次触发信号未处理挂起，则本次触发直接丢弃，称为超限）

