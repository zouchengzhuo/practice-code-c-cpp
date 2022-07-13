/*
 * @Date: 2022-07-12 08:45:36
 * @LastEditors: czzou
 * @LastEditTime: 2022-07-13 14:14:49
 * @FilePath: /practice-code-c-cpp/linux_event/libevent/3.1single_reactor_multi_worker/client.cpp
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sstream>
#include <strings.h>
#include <thread>
#define handle_error(msg) do{ perror(msg); exit(EXIT_FAILURE); } while(0)

void connect_and_send(){
    //step1. 创建一个socket，并连接到server
    int cfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8080);
    addr.sin_family = AF_INET;
    int ret = connect(cfd, (sockaddr*)&addr, sizeof(addr));
    if(ret!=0) handle_error("connect to server fail");
    //step2. 循环写、读
    std::thread::id tid =  std::this_thread::get_id();
    std::stringstream msg;
    msg << "msg from " << tid;
    std::string s = msg.str();
    char buf[1024];
    while (1)
    {
        ret = send(cfd, s.c_str(), s.size(), 0);
        if(ret == -1) handle_error("client send fail");
        printf("%d--->: %s \n", cfd, s.c_str());
        bzero(buf, sizeof(buf));
        ret = recv(cfd, buf, sizeof(buf), 0);
        if(ret == 0){
            close(cfd);
            break;
        }
        if(ret == -1){
            handle_error("client recv fail");
            break;
        }
        printf("%d<---: %s \n", cfd, buf);
    }
}

int main(){
    int thread_num = 10;
    for(int i=0;i<thread_num;i++){
        std::thread t(connect_and_send);
        t.detach();
    }
    sleep(20);
}