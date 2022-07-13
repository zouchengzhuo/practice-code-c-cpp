#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <strings.h>
#include "WorkerPool.h"

#define handle_error(msg)  do { perror(msg); exit(EXIT_FAILURE); } while (0)

WorkerPool pool(1);

struct reactor_event {
    int efd; //epoll fd
    int fd;  //被监听的fd
    void (*callback)(epoll_event *ev); //回调函数
};

void bind_reacotr_event(epoll_event *ev, int efd, int fd, void (*callback)(epoll_event *ev)){
    reactor_event *rev = (reactor_event*)malloc(sizeof(reactor_event));
    rev->fd = fd;
    rev->efd = efd;
    rev->callback = callback;
    ev->data.ptr = rev;
}

void unbind_reactor_event(epoll_event *ev){
    free(ev->data.ptr);
    ev->data.ptr = nullptr;
}

void data_handler(epoll_event *ev){
    reactor_event *rev = (reactor_event*)ev->data.ptr;
    int efd = rev->efd;
    int cfd = rev->fd;
    char buf[100];
    bzero(buf, sizeof(buf));
    while (1)
    {
        int size = recv(cfd, buf, sizeof(buf), 0);
        //连接关闭，从epoll中移除监听并关闭fd
        if(size == 0){
            //这里先 DEL 再 close，DEL操作有一定几率因为fd已经被close而报错，看着像执行语句被重排了
            printf("EPOLL_CTL_DEL fd:%d\n", cfd);
            if(epoll_ctl(efd, EPOLL_CTL_DEL, cfd, ev) != 0) handle_error("epoll del fail");
            printf("close fd:%d\n", cfd);
            close(cfd);
            return;
        }
        if(size == -1){
            if(errno == EWOULDBLOCK || errno == EAGAIN){
                break;
            }
            handle_error("recv fail");
        }
        printf("%d<---: %s \n", cfd, buf);
        //测试：单reactor单worker，sleep 1s 再返回，qps急剧下降
        sleep(1);
        //原样返回
        size = send(cfd, buf, sizeof(buf), 0);
        if(size == -1) perror("send to client fail");
        printf("%d--->: %s \n", cfd, buf);
    }
}

void data_handler_by_worker(epoll_event *ev){
    reactor_event* rev = (reactor_event*)(ev->data.ptr);
    printf("add to queue fd %d events: %u\n",rev->fd, ev->events);
    pool.addQueue([ev]{
        data_handler(ev);
    });
}
void connection_handler(epoll_event *ev){
    //接收客户端连接
    reactor_event *rev = (reactor_event*)ev->data.ptr;
    int efd = rev->efd;
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int cfd = 0;
    while (1)
    {
        cfd = accept(rev->fd, (sockaddr*)&addr, &addr_len);
        if(cfd == -1){
            if(errno == EWOULDBLOCK || errno == EAGAIN){
                break;
            }
            handle_error("accept fail");
        }
        //将客户端连接也设置为非阻塞的
        int origin_flags = fcntl(cfd, F_GETFL, 0);
        fcntl(cfd, F_SETFL, origin_flags|O_NONBLOCK);
        //绑定到epoll监听
        epoll_event bev;
        bev.events = EPOLLIN;
        //绑定线程池worker，加快任务处理速度
        //bind_reacotr_event(&bev, efd, cfd, data_handler);
        bind_reacotr_event(&bev, efd, cfd, data_handler_by_worker);
        int ret = epoll_ctl(efd, EPOLL_CTL_ADD, cfd, &bev);
        if(ret != 0) handle_error("epoll add client fd fail");
        printf("accept client fd %d\n", cfd);
    }
}


int main(){
    pool.start();
    //step1. 创建一个epoll fd
    int efd = epoll_create(10);
    //step2. 创建一个 server socket，并监听端口
    int sfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
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
    //要使用多worker，必须使用边缘触发模式，不然还需要判断event是否重复触发
    ev.events = EPOLLIN|EPOLLET;
    bind_reacotr_event(&ev, efd, sfd, connection_handler);
    ret = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &ev);
    if(ret != 0) handle_error("epoll add fail");
    //step4. 循环调用 epoll wait
    epoll_event evs[1];
    int ev_num = 0;
    while (1)
    {
        ev_num = epoll_wait(efd, evs, 10, -1); 
        if(ev_num == -1) handle_error(" epoll wait fail");
        int i = 0;
        //step5. 挨个处理有io事件发生的fd，直接使用绑定在 reactor event 上的回调来处理
        printf("epoll event num: %d\n", ev_num);
        //这种场景其实用边缘触发是最好的， 不然epoll一直在触发
        while (i < ev_num){
            epoll_event* ev = &evs[i];
            reactor_event* rev = (reactor_event*)ev->data.ptr;
            //printf("epoll event fd: %d, ev: %d\n", rev->fd, ev->events);
            rev->callback(ev);
            i++;
        }
    }
}