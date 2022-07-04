#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <vector> 
#include <string.h>

#define handle_error(msg)  do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define BUFFER_SIZE 1024
#define LISTEN_BACKLOG 4

int NonblockingListenINET(){
    //创建监听地址
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(8080);
    //创建服务端socket
    //这里在初始化的时候就设置为非阻塞的，man文档中描述 Linux 2.6.27 之后可以这样
    //网上很多代码都是创建之后再设置的
    // int flags = fcntl(sock, F_GETFL, 0);
    // fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    int sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    //绑定fd和addr
    if( bind(sfd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1 ) handle_error("bind inet error");
    //监听fd
    if( listen(sfd, LISTEN_BACKLOG) == -1 ) handle_error("listen inet error");
    return sfd;
}

int NonblockingAcceptAndRecv(int sfd){
    //保存接收到的client fd列表，每次循环中遍历 recv一次
    std::vector<int> client_fds;
    //用于接收数据的buf
    char buf[BUFFER_SIZE];
    //accept循环
    while (1)
    {
        //创建客户端fd
        sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int client_fd = accept(sfd, (sockaddr*)&addr, &len);
        if(client_fd == -1){
            int e = errno;
            //在监听socket非阻塞模式下，没有新的连接到来时，会返回这两个错误码。可认为是正常状态
            if(e != EAGAIN && e != EWOULDBLOCK){
                printf("fd %d accept fail, errno: %d \n", client_fd, e);
                return 0;
            }
            //如果打开此日志，因为是 非阻塞的监听socket，此日志会不断打出
            //printf("accept errno  %d\n", e);
        } else {
            //有了连接，把客户端fd设置为非阻塞模式，放入列表
            int origin_flags = fcntl(client_fd, F_GETFL, 0);
            fcntl(client_fd, F_SETFL, origin_flags|O_NONBLOCK);
            client_fds.push_back(client_fd);
            printf("fd %d accept success \n", client_fd);
        }
        //recv循环
        while (1)
        {
            if(client_fds.size() == 0) break;
            auto iter = client_fds.begin();
            while (iter != client_fds.end())
            {
                memset(&buf, 0, sizeof(buf));
                 //非阻塞模式下，有数据返回字节数，断开返回0，错误返回-1，其中 EAGAIN、EWOULDBLOCK 不是真的错，只是没数据
                int byte_size = recv(*iter, &buf, sizeof(buf), 0);
                if(byte_size == 0){
                    //连接断开
                    printf("fd %d disconnected\n", *iter);
                    close(*iter);
                    iter = client_fds.erase(iter);
                    continue;
                } else if(byte_size == -1){
                    int e = errno;
                    if(e != EAGAIN && e != EWOULDBLOCK){
                        //读取出错
                        printf("fd %d recv fail, errno: %d \n", *iter, e);
                        close(*iter);
                        iter = client_fds.erase(iter);
                        continue;
                    } else {
                        //因为是非阻塞的普通socket，任意socket没收到数据时，此日志都会不断打出
                        //printf("fd %d recv errno %d\n", *iter, e);
                        //读取没出错，但是也没数据，直接去处理下一个 client fd
                        iter++;
                        continue;
                    }
                } else {
                    //读到数据了，原样返回，然后处理下一个
                    printf("<-- fd, %d client->server:%s\n", *iter, buf);
                    send(*iter, &buf, sizeof(buf), 0);
                    iter++;
                }
            }
            break;
        }
    }
}

int main(){
    int sfd = NonblockingListenINET();
    NonblockingAcceptAndRecv(sfd);
}