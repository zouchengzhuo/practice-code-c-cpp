#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <set>
#include <signal.h>

#include "utils.h"

#define BUFFER_SIZE 1024
#define LISTEN_BACKLOG 2
#define handle_error(msg)  do { perror(msg); exit(EXIT_FAILURE); } while (0)

///////// 实现一个server，用select来管理接收连接和数据接收的过程

int ListenINET(){
    //create socket
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    //create address
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8080);
    //bind
    int ret = bind(sfd, (sockaddr*)&addr, sizeof(addr));
    if(ret != 0) handle_error("bind fail");
    ret = listen(sfd, LISTEN_BACKLOG);
    if(ret != 0) handle_error("listen fail");
    return sfd;
}

int SelectINET(int sfd){
    signal(SIGPIPE, SIG_IGN);
    //用于存储原始的绑定到select上的fd，不每次select的时候都重新搞，有变更的时候变这个然后设置给select就好了
    fd_set origin_set;
    //!!!重要!!! 不然会带一堆原本内存里存在的东西
    FD_ZERO(&origin_set);
    //记录最大的fd值，用于select
    int max_fd = 0;
    //记录select中的fd列表，用于有消息返回后的遍历
    std::set<int> fds;
    //首先将sfd设置到select的监听列表上
    AddFd(origin_set, fds, max_fd, sfd);
    //存储各种unistd函数的返回值
    int ret = 0;
    //存储用于accept操作的客户端addr结构
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    //存储socket从客户端收到的数据
    char buf[BUFFER_SIZE];
    //select循环
    while (1)
    {
        //这里只写一下读取数据的select，写数据和异常的select暂时不做处理，最后一个参数是wait时间，传null永久等待
        fd_set readfds = origin_set;
        fd_set writefds = origin_set;
        fd_set exfds = origin_set;
        //打印出当前 select 处理的 fd_set，可以观察下规律
        printf("start select fd_set: \n");
        PrintSet(&origin_set);
        ret = select( max_fd + 1, &readfds, &writefds, &exfds, nullptr);
        printf("fds size:%d,  select ret: %d , max_fd: %d \n", fds.size(), ret, max_fd);
        //超时才会为0，永久等待这里一般是不会为0的，直接continue
        if(ret == 0) continue;
        if(ret == -1) handle_error("select fail");
        std::set<int>::iterator iter = fds.begin();
        while (iter!=fds.end())
        {
            if(!FD_ISSET(*iter, &readfds)){
                iter++;
                continue;
            }
            printf("read fd: %d\n", *iter);
            //有数据，对于监听socket，执行accept操作，对于数据socket，执行接收数据操作
            if(*iter == sfd){
                int cfd = accept(sfd, (struct sockaddr*)&addr, &addr_len);
                if(cfd == -1){
                    printf("accept error, errno: %d \n", errno);
                    iter++;
                    continue;
                }
                //监听socket收到了新的客户端连接fd，设置给select监听
                AddFd(origin_set, fds, max_fd, cfd);
                iter++;
                printf("connected fd: %d \n", cfd);
                continue;
            }
            //普通socket收到了数据，正常处理
            bzero(&buf, sizeof(buf));
            ret = recv(*iter, &buf, sizeof(buf), 0);
            if(ret == 0){
                printf("fd %d disconnected \n", *iter);
                int cfd = *iter;
                //断开连接
                RemoveFd(origin_set, fds, max_fd, iter);
                close(cfd);
                continue;
            }
            //接收错误
            if(ret == -1){
                printf("fd %d recv errno: %d \n", *iter, errno);
                //可读了还是出错，断开连接
                int cfd = *iter;
                RemoveFd(origin_set, fds, max_fd, iter);
                close(cfd);
                continue;
            }
            //接收到数据，打印并返回
            printf("<--- client -> server: %s \n", buf);
            //原样返回
            //！！！注意：此示例并没有把socket设置为 nonblocking，也没有在select中判断可写状态，写操作有可能造成阻塞。
            //实际使用时要么非阻塞模式，要么判断可写后再写数据。
            ret = send(*iter, &buf, sizeof(buf), 0);
            if(ret == -1){
                printf("fd %d send errno: %d \n", *iter, errno);
                iter++;
                continue;
            }
            printf("---> server -> client: %s \n", buf);
            iter++;
        }
    }
    close(sfd);
}

int main(){
   int sfd =  ListenINET();
   SelectINET(sfd);
}