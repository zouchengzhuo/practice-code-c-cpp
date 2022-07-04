## 标准库操作符函数结构体
标准库还定义了一堆对应基本操作符的函数结构体，如：  
- 算术函数  
std::plus\<Type> 、std::minus\<Type>等
- 关系函数  
std::equal_to\<Type>等
- 逻辑函数  
std::logical_or<Type>、std::logical_not<Type>等  


## 标准库函数对象适配器
binder  
绑定器，绑定固定参数，获得更少参数的函数，类似于js里边的bind，但是只支持一个参数的函数，或者两个参数的函数  
std::bind1nd、 std::bind2nd，分别绑定固定数据到第1、第2个参数  

求反器  
将函数的bool值求反，与binder类似，也只支持1个或2个参数的函数  
not1  
not2

