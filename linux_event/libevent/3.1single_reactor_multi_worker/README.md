<!--
 * @Date: 2022-07-12 08:38:39
 * @LastEditors: czzou
 * @LastEditTime: 2022-07-13 08:16:11
 * @FilePath: /practice-code-c-cpp/linux_event/libevent/3.single_reactor_multi_worker/README.md
-->
单reactor单worker的模式的demo，都默认数据到达后的逻辑是 读→(处理)→写，处理的过程被忽略了，总是原样返回。  
如果实际上处理的过程是一个比较消耗CPU的过程，或者是一个会造成线程阻塞的过程，那么会造成什么情况呢？  
实验一下就知道，例如sleep 1s再回写数据，qps会急剧下降，相对于原来的超高qps，现在只能做到1s处理一个请求。  

这个demo是将数据读写的过程放到worker里边去做了，可以加速处理，但是在处理的过程中，会有一些core，暂时没有找到原因。  
网上的多worker模式，一般数据读写是不放在worker里的，只有解包、业务逻辑、组包，的过程放在worker里边，可以尝试一下新的。