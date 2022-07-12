#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <strings.h>
#define handle_error(msg)  do { perror(msg); exit(EXIT_FAILURE); } while (0)

void handle_connection(int efd, int sfd){
    //接收客户端连接
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int cfd = accept(sfd, (sockaddr*)&addr, &addr_len);
    if(cfd == -1) handle_error("accept fail");
    //绑定到epoll监听
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = cfd;
    int ret = epoll_ctl(efd, EPOLL_CTL_ADD, cfd, &ev);
    if(ret != 0) handle_error("epoll add client fd fail");
    printf("accept client fd %d\n", cfd);
}

void handle_data(int efd, int cfd, epoll_event *ev){
    char buf[1024];
    bzero(&buf, sizeof(buf));
    int size = recv(cfd, buf, sizeof(buf), 0);
    //连接关闭，或者出错，从epoll中移除监听并关闭fd
    //这里如果是非阻塞socket，那么-1时不应该执行此操作
    if(size == 0 || size == -1){
        if(epoll_ctl(efd, EPOLL_CTL_DEL, cfd, ev) != 0){
            close(cfd);
            handle_error("epoll del fail");
        }
        return;
    }
    printf("<---: %s \n", buf);
    //原样返回
    size = send(cfd, buf, sizeof(buf), 0);
    if(size == -1) handle_error("send to client fail");
    printf("--->: %s \n", buf);
}

int main(){
    //step1. 创建一个epoll fd
    int efd = epoll_create(10);
    //step2. 创建一个 server socket，并监听端口
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    //监听 0.0.0.0
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8080);
    int ret = bind(sfd, (sockaddr*)&addr, sizeof(addr));
    if(ret != 0) handle_error("bind fail");
    ret = listen(sfd, 2);
    if(ret != 0) handle_error("listen fail");
    printf("start listen\n");
    //step3. 将 sfd 添加到 epoll 监听
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = sfd;
    ret = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &ev);
    if(ret != 0) handle_error("epoll add fail");
    //step4. 循环调用 epoll wait
    epoll_event evs[10];
    int ev_num = 0;
    while (1)
    {
        ev_num = epoll_wait(efd, evs, 10, -1); 
        if(ev_num == -1) handle_error(" epoll wait fail");
        int i = 0;
        //step5. 挨个处理有io事件发生的fd，分情况处理：若是 sfd，则拿到新的客户端fd并加入监听；若是 client fd，则读写数据
        while (i < ev_num){
            epoll_event* ev = &evs[i];
            if(ev->data.fd == sfd){
                //如果是 server fd，进行客户端连接获取、epoll 添加监听的操作
                handle_connection(efd, sfd);
            } else {
                //如果是 client fd，进行数据读写操作
                handle_data(efd, ev->data.fd, ev);
            }
            i++;
        }
    }
}