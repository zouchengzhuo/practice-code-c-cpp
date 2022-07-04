# 基础的std::future和std::async

std::async的调度策略：  
- 默认由系统调度合适开始异步任务，任何时间都有可能开启
- 调用get会强制开启
- 如果传入 std::launch::async，则会立即开始异步任务，不行则抛异常；由这个构造的future，在其生命周期结束的地方，会等到它执行完毕
- 如果传入std::launch::deferred，则如果不调get绝对不会开启

std::future的wait策略

- wait会强制开启异步任务，等待直到异步任务执行结束
- wait_for，等待一段时间，不强制开启异步任务
- wait_until，等到某个时间点，不强制开启异步任务

# std::shared_future
可以多次get的future

# 在线程中运行类成员函数

https://cloud.tencent.com/developer/ask/43082

Person p;
auto f = std::async(&Person::sum, &p, xxx);
auto f = std::async(std::launch::async, &Person::sum, &p, xxx);