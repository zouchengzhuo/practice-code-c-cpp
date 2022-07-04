# notify_all
- 基本原理类似于一个，通过notify触发而不是sleep指定的时间来触发的，轮询加锁判断readyFlag
- 通知到所有线程时，是获取锁的线程才能突破wait，还是所有线程都突破？ 答案：所有
- 两种wait，何时阻塞
- condition_variable_any 实例
