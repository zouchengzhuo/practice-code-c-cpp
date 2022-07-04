# thread & promise

# mutex & lock

## 同时锁定多个

## unique_lock
作用与std::lock_guard类似，也会在生命周期结束时自动unlock。  
区别是可控性更强：  
- 生命周期期间不一定lock住了一个mutex，可以用owns_lock()，或者bool()来查询当前是否锁住了
- 可以与try_to_lock配合使用， std::unique_lock< std::mutex> ul(mutex, std::try_to_lock)，尝试获取lock，但是不阻塞
- 可以传入一个时间，类似try_lock_for，尝试在一段时间内锁定（待验证，也许gcc4.8不行）std::unique_lock< std::timed_mutex> ul(t_mutex, std::chrono::seconds(1))
- 可以传入defer_lock，初始化但不锁住，后续手动锁，std::unique_lock< std::mutex> ul(mutex, std::defer_lock)， 后续ul.lock()
- 提供release来释放，或者转移lock，此函数返回一个指向关联的mutex，并且unlock它；与之相对，ul.mutex()返回mutex的指针，但是不unlock

## std::once_flag 只执行一次
std::once_flag用来提供一个线程安全的，多线程环境下的数据初始化，避免出现数据冲突问题。  
```cpp
std::once_flag oc;
std::call_once(oc, callable);

```