#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <vector> 
#include <map> 
#include <string.h>
#include <time.h>

//发送、接收缓冲区大小
#define BUFFER_SIZE 1024
//客户端连接数量
#define CLIENT_NUM 5 
//请求发送间隔
#define SEND_TIME_DIFF 0
int NonblockingConnectSendINET(){
    //初始化10个客户端，循环发起连接并发送、接收数据，其中因为服务端连接队列长度有限，有两个client会一直卡在连接中直到超时。
    //未连接的fd
    std::vector<int> origin_client_fds;
    //已成功建立连接的fd，记录fd和最近一次发消息的时间，每10s发一次消息
    std::map<int, long> connected_fds;
    sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    remote_addr.sin_port = htons(8080);
    //用于发送和接收数据的buf
    char send_buf[BUFFER_SIZE], recv_buf[BUFFER_SIZE];
    //初始化10个非阻塞客户端socket
    for(int i=0;i<CLIENT_NUM;i++){
        int cfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
        origin_client_fds.push_back(cfd);
    }
    while (1)
    {
        //遍历未建立连接的fd，尝试建立连接
        std::vector<int>::iterator iter = origin_client_fds.begin();
        while (iter != origin_client_fds.end())
        {
            int ret = connect(*iter, (sockaddr*)&remote_addr, sizeof(remote_addr));
            //连接成功，插入已连接map，从未连接队列中去掉，下一个
            if(ret == 0){
                connected_fds.insert(std::make_pair(*iter,0));
                printf("fd %d connect success \n", *iter);
                iter = origin_client_fds.erase(iter);
                continue;
            }
            if(ret == -1){
                int e = errno;
                if(e == EAGAIN || e == EALREADY || e == EINPROGRESS){
                    //非阻塞模式下，未真正失败的几个状态
                    iter++;
                    continue;
                } else {
                    //连接失败
                    printf("fd %d connect fail, errno %d \n", *iter, e);
                    iter = origin_client_fds.erase(iter);
                    continue;
                }
            }
            iter++;
        }
        //遍历已建立连接的fd，尝试发送请求、接收数据
        std::map<int, long>::iterator miter = connected_fds.begin();
        while (miter != connected_fds.end())
        {
            time_t now;
            time(&now);
            //每隔多久发一次请求
            int ret;
            if(now - miter->second > SEND_TIME_DIFF){
                //超过10s，尝试发送数据
                bzero(send_buf, sizeof(send_buf));
                std::string msg = "msg from client fd " + std::to_string(miter->first);
                memcpy(send_buf, msg.c_str(), msg.size());
                ret = send(miter->first, send_buf, sizeof(send_buf), 0);
                //发送失败
                if(ret < 0){
                    printf("fd: %d send fail: %d \n", miter->first, errno);
                } else {
                    //发送成功，
                    printf("fd %d --> client->server:%s\n", miter->first, send_buf);
                }
                //标记发送成功时间点
                miter->second = now;
            }
            //尝试接收数据
            memset(recv_buf, 0, sizeof(recv_buf));
            ret = recv(miter->first, &recv_buf, sizeof(recv_buf), 0);
            if(ret < 0){
                miter++;
                int e = errno;
                //非阻塞，未读取到数据
                if(e==EAGAIN || e==EWOULDBLOCK){
                    continue;
                }
                printf("fd %d client recv error: %d \n", miter->first, e);
            } else if(ret == 0){
                //连接断开，剔除掉
                printf("fd %d recv disconnect\n", miter->first);
                connected_fds.erase(miter);
                miter++;
                continue;
            } else{
                printf("fd %d <-- server->client:%s\n", miter->first, recv_buf);
                miter++;
            }
        }
    }
}

int main(){
    NonblockingConnectSendINET();
}