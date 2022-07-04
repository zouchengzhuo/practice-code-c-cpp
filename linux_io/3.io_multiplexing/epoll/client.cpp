#include <stdio.h>
#include <iostream>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
#define CLIENT_NUM 5
#define EPOLL_MAX_EVENTS 10
#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

///////// 实现一个client，用poll来管理发起连接和收发数据的问题

void MakeFdNonblocking(int &fd){
    int origin_flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, origin_flags|O_NONBLOCK);
}

int PollClient(bool etMode)
{
    //epoll的fd
    int efd = epoll_create(10);
    //远程监听的地址
    sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    remote_addr.sin_port = htons(8080);
    //存储各种unistd函数的返回值
    int ret = 0;
    //存储从服务端收到的数据
    char recv_buf[BUFFER_SIZE], send_buf[BUFFER_SIZE];
    //存储epoll返回的events
    epoll_event evs[EPOLL_MAX_EVENTS];
    //初始化 CLIENT_NUM 个非阻塞socket
    for (int i = 0; i < CLIENT_NUM; i++)
    {
        int cfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        if (cfd == -1)
        {
            printf("create client socket fail, errno: %d\n", errno);
            continue;
        }
        //需要接收读和写的事件
        epoll_event ev{EPOLLIN | EPOLLOUT, {.fd=cfd}};
        if(epoll_ctl(efd, EPOLL_CTL_ADD, cfd, &ev) != 0) handle_error("epoll add");
        //连接，后面用poll判断可写状态
        connect(cfd, (sockaddr *)&remote_addr, sizeof(remote_addr));
    }
    while (1)
    {
        ret = epoll_wait(efd, evs, EPOLL_MAX_EVENTS, -1 );
        printf("====epoll ret==== %d\n", ret);
        //超时才会为0，直接continue
        if (ret == 0)
            continue;
        if (ret == -1)
            handle_error("epoll fail");
        //如果有可写的，代表连接已成功，写入数据
        int i = 0;
        int ev_num = ret;
        while (i < ev_num)
        {
            int cfd = evs[i].data.fd;
            printf("===== cfd ======= %d\n", cfd);
            //可写
            if (EPOLLOUT & evs[i].events )
            {
                int connect_ret = -1;
                int connect_ret_len = sizeof(connect_ret);
                ret = getsockopt(cfd, SOL_SOCKET, SO_ERROR, &connect_ret, (socklen_t *)&connect_ret_len);
                if (ret == -1)
                {
                    printf("fd %d getsockopt errno: %d \n", cfd, errno);
                    i++;
                    continue;
                }
                //参考connect文档，可写
                if (connect_ret == 0)
                {
                    std::string msg = "hello from fd" + std::to_string(cfd);
                    bzero(send_buf, sizeof(send_buf));
                    memcpy(send_buf, msg.c_str(), msg.size());
                    ret = send(cfd, &send_buf, sizeof(send_buf), 0);
                    if (ret == -1)
                    {
                        printf("fd %d send errno: %d \n", cfd, errno);
                    }
                    printf("---> client -> server: %s \n", send_buf);
                }
                else
                {
                    printf("fd %d getsockopt so_error: %d \n", cfd, connect_ret);
                }
            }
            i++;
        }
        //如果有可读的，读出数据
        //可以放到一个循环里边用 do...while(0)+break一次性完成，这里练习代码为了逻辑清晰点分开写
        i=0;
        while (i<ev_num)
        {
            int cfd = evs[i].data.fd;
            //可读
            if (EPOLLIN & evs[i].events)
            {
                //如果是etMode，要读完数据，不然可能导致fd卡在中间状态
                int recv_size = 0;
                do{
                    bzero(&recv_buf, sizeof(recv_buf));
                    ret = recv(cfd, &recv_buf, sizeof(recv_buf), 0);
                    recv_size = ret;
                    if (ret == 0)
                    {
                        //断开连接
                        if(epoll_ctl(efd, EPOLL_CTL_DEL, cfd, nullptr) != 0) handle_error("close fail");
                        close(cfd);
                        break;
                    }
                    //接收错误
                    if (ret == -1)
                    {
                        if (errno != EAGAIN && errno != EWOULDBLOCK){
                             printf("fd %d recv errno: %d \n", cfd, errno);
                            //断开连接
                            if(epoll_ctl(efd, EPOLL_CTL_DEL, cfd, nullptr)!=0) handle_error("epoll del");
                            close(cfd);
                        }
                        break;
                    }
                    //接收到数据，打印并返回
                    printf("<--- server -> client: %s \n", recv_buf);
                } while (etMode && recv_size>0);
            }
            i++;
        }
    }
}

int main()
{
    PollClient(true);
}