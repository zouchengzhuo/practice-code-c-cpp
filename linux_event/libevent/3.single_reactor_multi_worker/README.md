<!--
 * @Date: 2022-07-12 08:38:39
 * @LastEditors: czzou
 * @LastEditTime: 2022-07-14 02:49:30
 * @FilePath: /practice-code-c-cpp/linux_event/libevent/3.single_reactor_multi_worker/README.md
-->
单reactor单worker的模式的demo，都默认数据到达后的逻辑是 读→(处理)→写，处理的过程被忽略了，总是原样返回。  
如果实际上处理的过程是一个比较消耗CPU的过程，或者是一个会造成线程阻塞的过程，那么会造成什么情况呢？  
实验一下就知道，例如sleep 1s再回写数据，qps会急剧下降，相对于原来的超高qps，现在只能做到1s处理一个请求。  

这个demo是将数据读写的过程放到worker里边去做了，可以加速处理，但是在处理的过程中，会有一些core，或者client/server端表现不符合预期的情况，总结一下：
   
实现易错点：  
1. epoll_wait 的时候，设置最大获取10个epoll_event，但是传进去的数组长度只有1，这样取epoll_event的时候会越界，造成未定义操作
2. event_loop中调用callback的时候，是异步的，最开始的实现是把 epoll_event 的指针传递给异步线程， 会导致一个问题，指针指向的 epoll_event 会在循环中被覆盖，应该修改为传递不会被覆盖的 reactor_event 指针
3. 不能因为 epoll_ctl EPOLL_CTL_ADD 的时候只监听了 EPOLLIN，就不判断 epoll_event.events 的值，这个还有可能返回表示错误的 events，此时去读数据可能导致未定义行为

除了这种模式，网上的文章描述的单reactor多worker模式，一般数据读写是不放在worker里的，只有解包、业务逻辑、组包，的过程放在worker里边，3.1single_reactor_multi_worker目录下实践一下这种模式。  