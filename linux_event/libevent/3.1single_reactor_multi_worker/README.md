<!--
 * @Date: 2022-07-12 08:38:39
 * @LastEditors: czzou
 * @LastEditTime: 2022-07-14 02:46:53
 * @FilePath: /practice-code-c-cpp/linux_event/libevent/3.1single_reactor_multi_worker/README.md
-->
单reactor单worker的模式的demo，都默认数据到达后的逻辑是 读→(处理)→写，处理的过程被忽略了，总是原样返回。  
如果实际上处理的过程是一个比较消耗CPU的过程，或者是一个会造成线程阻塞的过程，那么会造成什么情况呢？  
实验一下就知道，例如sleep 1s再回写数据，qps会急剧下降，相对于原来的超高qps，现在只能做到1s处理一个请求。  

这个demo将模拟的 解包→处理→组包 的工作放到 worker 线程中做，但是读写操作仍然放在主线程中，主线程 event_loop 中对每个 client fd 额外创建并监听一个 eventfd，用于通知请求处理完毕的事件。