TCP Echo Server Example in C++ Using Epoll  

一个简单的tcp服务器的示例，接受tcp连接，接收数据并原样返回。例子演示了epoll的使用，它比poll更高效。  
与轮询不同的是，在epoll中，并非每次进行等待调用时都会传递所有需要监视的句柄事件。  
Epoll使用事件注册，可以在其中添加、修改或删除要观看的事件。这使得它在有大量活动需要观看时非常有效。  

g++ epoll_client.cpp  -o epoll_client -std=c++11 -lpthread
g++ epoll_server.cpp  -o epoll_server -std=c++11 -lpthread