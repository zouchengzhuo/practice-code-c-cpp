#include <stdio.h>
#include <iostream>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <set>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "utils.h"

#define BUFFER_SIZE 1024
#define CLIENT_NUM 5
#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

///////// 实现一个client，用select来管理发起连接和收发数据的问题

int SelectClient()
{
    //用于存储原始的绑定到select上的fd，不每次select的时候都重新搞，有变更的时候变这个然后设置给select就好了
    fd_set origin_set;
    //!!!重要!!! 不然会带一堆原本内存里存在的东西
    FD_ZERO(&origin_set);
    //记录最大的fd值，用于select
    int max_fd = 0;
    //记录select中的fd列表，用于有消息返回后的遍历
    std::set<int> fds;
    //远程监听的地址
    sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    remote_addr.sin_port = htons(8080);
    //存储各种unistd函数的返回值
    int ret = 0;
    //存储从服务端收到的数据
    char recv_buf[BUFFER_SIZE], send_buf[BUFFER_SIZE];
    //初始化 CLIENT_NUM 个非阻塞socket
    for (int i = 0; i < CLIENT_NUM; i++)
    {
        int cfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        if (cfd == -1)
        {
            printf("create client socket fail, errno: %d\n", errno);
            continue;
        }
        AddFd(origin_set, fds, max_fd, cfd);
        //连接，后面用select判断可写状态
        connect(cfd, (sockaddr *)&remote_addr, sizeof(remote_addr));
    }
    while (1)
    {
        fd_set read_set = origin_set, wirte_set = origin_set;
        //select监听超时时间
        timeval select_timeout{5, 0};
        ret = select(max_fd + 1, &read_set, &wirte_set, nullptr, &select_timeout);
        printf("====select ret==== %d\n", ret);
        //超时才会为0，直接continue
        if (ret == 0)
            continue;
        if (ret == -1)
            handle_error("select fail");
        //如果有可写的，代表连接已成功，写入数据
        std::set<int>::iterator iter = fds.begin();
        while (iter != fds.end())
        {
            //可写
            if (FD_ISSET(*iter, &wirte_set))
            {
                int connect_ret = -1;
                int connect_ret_len = sizeof(connect_ret);
                ret = getsockopt(*iter, SOL_SOCKET, SO_ERROR, &connect_ret, (socklen_t *)&connect_ret_len);
                if (ret == -1)
                {
                    printf("fd %d getsockopt errno: %d \n", *iter, errno);
                    iter++;
                    continue;
                }
                //参考connect文档，可写
                if (connect_ret == 0)
                {
                    std::string msg = "hello from fd" + std::to_string(*iter);
                    bzero(send_buf, sizeof(send_buf));
                    memcpy(send_buf, msg.c_str(), msg.size());
                    ret = send(*iter, &send_buf, sizeof(send_buf), 0);
                    if (ret == -1)
                    {
                        printf("fd %d send errno: %d \n", *iter, errno);
                    }
                    printf("---> client -> server: %s \n", send_buf);
                }
                else
                {
                    printf("fd %d getsockopt so_error: %d \n", *iter, connect_ret);
                }
            }
            iter++;
        }
        //如果有可读的，读出数据
        //可以放到一个循环里边用 do...while(0)+break一次性完成，这里练习代码为了逻辑清晰点分开写
        iter = fds.begin();
        while (iter != fds.end())
        {
            //可读
            if (FD_ISSET(*iter, &read_set))
            {
                bzero(&recv_buf, sizeof(recv_buf));
                ret = recv(*iter, &recv_buf, sizeof(recv_buf), 0);
                if (ret == 0)
                {
                    int cfd = *iter;
                    //断开连接
                    RemoveFd(origin_set, fds, max_fd, iter);
                    close(cfd);
                    continue;
                }
                //接收错误
                if (ret == -1)
                {
                    printf("fd %d recv errno: %d \n", *iter, errno);
                    iter++;
                    continue;
                }
                //接收到数据，打印并返回
                printf("<--- server -> client: %s \n", recv_buf);
            }
            iter++;
        }
    }
}

int main()
{
    SelectClient();
}