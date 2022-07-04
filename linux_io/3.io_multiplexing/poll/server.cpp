#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <vector>
#include <signal.h>

#define BUFFER_SIZE 1024
#define LISTEN_BACKLOG 2
#define handle_error(msg)  do { perror(msg); exit(EXIT_FAILURE); } while (0)

///////// 实现一个server，用poll来管理接收连接和数据接收的过程

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

int PollINET(int sfd){
    signal(SIGPIPE, SIG_IGN);
    //用于存储交给poll监视的fds
    std::vector<pollfd> fds;
    //首先将sfd设置到poll的监听列表上
    fds.push_back({sfd, POLLIN});
    //存储各种unistd函数的返回值
    int ret = 0;
    //存储用于accept操作的客户端addr结构
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    //存储socket从客户端收到的数据
    char buf[BUFFER_SIZE];
    //poll循环
    while (1)
    {

        printf("start poll \n");
        //-1无限阻塞，相当于select的time传个nullptr
        ret = poll( &fds[0], fds.size(), 2000);
        printf("fds size:%d,  poll ret: %d \n", fds.size(), ret);
        //超时才会为0，永久等待这里一般是不会为0的，直接continue
        if(ret == 0) continue;
        if(ret == -1) handle_error("poll fail");
        std::vector<pollfd>::iterator iter = fds.begin();
        while (iter!=fds.end())
        {
            if(!(POLLIN & iter->revents)){
                iter++;
                continue;
            }
            printf("read fd: %d\n", iter->fd);
            //有数据，对于监听socket，执行accept操作，对于数据socket，执行接收数据操作
            if(iter->fd == sfd){
                int cfd = accept(sfd, (struct sockaddr*)&addr, &addr_len);
                if(cfd == -1){
                    printf("accept error, errno: %d \n", errno);
                    iter++;
                    continue;
                }
                //监听socket收到了新的客户端连接fd，设置给poll监听
                fds.push_back({cfd, POLLIN});
                iter++;
                printf("connected fd: %d \n", cfd);
                continue;
            }
            //普通socket收到了数据，正常处理
            bzero(&buf, sizeof(buf));
            ret = recv(iter->fd, &buf, sizeof(buf), 0);
            if(ret == 0){
                printf("fd %d disconnected \n", iter->fd);
                int cfd = iter->fd;
                //断开连接
                iter = fds.erase(iter);
                close(cfd);
                continue;
            }
            //接收错误
            if(ret == -1){
                printf("fd %d recv errno: %d \n", iter->fd, errno);
                int cfd = iter->fd;
                //断开连接
                iter = fds.erase(iter);
                close(cfd);
                continue;
            }
            //接收到数据，打印并返回
            printf("<--- client -> server: %s \n", buf);
            //原样返回
            //！！！注意：此示例并没有把socket设置为 nonblocking，也没有在poll中判断可写状态，写操作有可能造成阻塞。
            //实际使用时要么非阻塞模式，要么判断可写后再写数据。
            ret = send(iter->fd, &buf, sizeof(buf), 0);
            if(ret == -1){
                printf("fd %d send errno: %d \n", iter->fd, errno);
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
   PollINET(sfd);
}