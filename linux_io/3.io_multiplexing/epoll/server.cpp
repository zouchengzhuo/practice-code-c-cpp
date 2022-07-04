#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <vector>
#include <signal.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
#define LISTEN_BACKLOG 2
#define EPOLL_MAX_EVENTS 10
#define handle_error(msg)  do { perror(msg); exit(EXIT_FAILURE); } while (0)

///////// 实现一个server，用epoll来管理接收连接和数据接收的过程

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

void MakeFdNonblocking(int &fd){
    int origin_flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, origin_flags|O_NONBLOCK);
}

int EpollINET(int sfd, bool etMode){
    signal(SIGPIPE, SIG_IGN);

    //step1.创建一个epoll，后面的size是给系统的建议初始size，并不是指定固定的size
    int efd = epoll_create(10);
    uint32_t event = etMode ? EPOLLIN|EPOLLET : EPOLLIN;
    if(etMode) MakeFdNonblocking(sfd);
    epoll_event ev{ event, {.fd=sfd} };
    //step2.向epoll中注册server fd
    if(epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &ev) != 0) handle_error("epoll add");
    //存储各种unistd函数的返回值
    int ret = 0;
    //存储用于accept操作的客户端addr结构
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    //存储socket从客户端收到的数据
    char buf[BUFFER_SIZE];
    //创建用于接收event的 epoll_event struct数组
    epoll_event evs[EPOLL_MAX_EVENTS];
    //epoll循环
    while (1)
    {
        printf("start epoll \n");
        //step3.epoll阻塞等待事件发生，ret返回的是有io事件发生的个数。 -1无限阻塞，相当于select的time传个nullptr
        ret = epoll_wait(efd, evs, EPOLL_MAX_EVENTS, -1);
        printf("epoll ret: %d \n", ret);
        //超时才会为0，永久等待这里一般是不会为0的，直接continue
        if(ret == 0) continue;
        if(ret == -1) handle_error("epoll fail");
        int ev_num = ret;
        int i = 0;
        //与select和poll不同的是，这里只需要遍历epoll返回的有事件的数量就可以了
        while (i < ev_num)
        {
            epoll_event *c_ev = &evs[i];
            int cfd = c_ev->data.fd;
            if(!(EPOLLIN & c_ev->events)){
                i++;
                continue;
            }
            printf("read fd: %d\n", cfd);
            //step4.读取有io事件发生的fd上的数据，server socket和client socket分不同情况处理
            //有数据，对于监听socket，执行accept操作，对于数据socket，执行接收数据操作
            if(cfd == sfd){
                int cfd = accept(sfd, (struct sockaddr*)&addr, &addr_len);
                if(cfd == -1){
                    printf("accept error, errno: %d \n", errno);
                    i++;
                    continue;
                }
                //如果是边缘触发模式，需要将socket设置为非阻塞的
                if(etMode) MakeFdNonblocking(cfd);
                //监听socket收到了新的客户端连接fd，设置给epoll监听
                epoll_event ev{ event, {.fd=cfd} };
                if(epoll_ctl(efd, EPOLL_CTL_ADD, cfd, &ev)!=0) handle_error("epoll add");
                i++;
                printf("connected fd: %d \n", cfd);
                continue;
            }
            //如果是 ET 边缘触发模式，需要不断读取内容，直到读完
            //因为ET模式不会重复触发事件，不读完的话否则可能导致连接状态卡住
            //普通socket收到了数据，正常处理
            int recv_size = 0;
            do{
                bzero(&buf, sizeof(buf));
                ret = recv(cfd, &buf, sizeof(buf), 0);
                recv_size = ret;
                if(ret == 0){
                    printf("fd %d disconnected \n", cfd);
                    if(epoll_ctl(efd, EPOLL_CTL_DEL, cfd, c_ev)!=0) handle_error("epoll del");
                    close(cfd);
                    break;
                }
                //接收错误
                if(ret == -1){
                    if (errno != EAGAIN && errno != EWOULDBLOCK){
                        printf("fd %d recv errno: %d \n", cfd, errno);
                        //断开连接
                        if(epoll_ctl(efd, EPOLL_CTL_DEL, cfd, c_ev)!=0) handle_error("epoll del");
                        close(cfd);
                    }
                    break;
                }
                //接收到数据，打印并返回
                printf("<--- client -> server: %s \n", buf);
                //原样返回
                ret = send(cfd, &buf, sizeof(buf), 0);
                if(ret == -1){
                    printf("fd %d send errno: %d \n", cfd, errno);
                    break;;
                }
                printf("---> server -> client: %s \n", buf);
            } while (etMode && recv_size > 0);
            i++;
        }
    }
    close(sfd);
}

int main(){
   int sfd =  ListenINET();
   EpollINET(sfd, true);
}