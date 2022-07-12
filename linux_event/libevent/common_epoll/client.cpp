
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
        printf("--->: %s \n", s.c_str());
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
        printf("<---: %s \n", buf);
    }
}

int main(){
    std::thread t1(connect_and_send);
    std::thread t2(connect_and_send);
    t1.join();
    t2.join();
}