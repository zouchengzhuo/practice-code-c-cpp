#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/eventfd.h>
#include <strings.h>
#include "WorkerPool.h"
#include "SafeQueue.h"

#define handle_error(msg)  do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define THREAD_POLL_SIZE 10

WorkerPool pool(THREAD_POLL_SIZE);

struct reactor_event {
    int epoll_fd; //epoll fd
    int client_fd;  //被监听的fd
    int rsp_event_fd; //有处理完毕的请求，通知响应的 event fd
    SafeQueue<std::string> *req_queue;
    SafeQueue<std::string> *rsp_queue;
    void (*callback)(epoll_event *ev); //回调函数

};

void bind_reacotr_event(epoll_event *ev, int epoll_fd, int client_fd, int rsp_event_fd, void (*callback)(epoll_event *ev), 
    SafeQueue<std::string>* req_queue, SafeQueue<std::string>* rsp_queue){
    //reactor_event *rev = (reactor_event*)malloc(sizeof(reactor_event));
    reactor_event *rev = new reactor_event();
    rev->client_fd = client_fd;
    rev->epoll_fd = epoll_fd;
    rev->rsp_event_fd = rsp_event_fd;
    rev->callback = callback;
    rev->req_queue = req_queue;
    rev->rsp_queue = rsp_queue;
    ev->data.ptr = rev;
    //printf("reactor_event of fd %d is %p\n", client_fd, rev);
}

void unbind_reactor_event(epoll_event *ev){
    reactor_event *rev = new reactor_event();
    free(rev->req_queue);
    free(rev->rsp_queue);
    free(ev->data.ptr);
    rev->req_queue = nullptr;
    rev->rsp_queue = nullptr;
    ev->data.ptr = nullptr;
}


/**
 * @description: worker中的处理函数，模拟解包、处理、组包的过程
 * @param {epoll_event} *ev
 * @return {*}
 */
void data_handler(reactor_event *rev){
    //printf("in data_handler... \n");
    //reactor_event *rev = (reactor_event*)ev->data.ptr;
    int efd = rev->epoll_fd;
    int cfd = rev->client_fd;
    int evfd = rev->rsp_event_fd;
    //模拟 worker 线程请求处理的过程：
    //解包：从req中取出数据
    std::string msg;
    if(!rev->req_queue->shift_no_wait(msg)){
        printf("req queue is empty... cfd:%d rev:%p req_queue p:%p \n", cfd, rev, rev->req_queue);
        return;
    }
    //处理：sleep 1s
    sleep(1);
    //组包：把数据放入rsp
    //printf("put msg to rsp cfd: %d evfd: %d ... \n", cfd, evfd);
    rev->rsp_queue->push(msg);
    //printf("put msg to rsp cfd: %d evfd: %d ... done rev:%p p:%p size: %d\n", cfd, evfd, rev, rev->rsp_queue, rev->rsp_queue->size());
    //通知：利用 eventfd 通知给 reactor 线程
    //TODO：是否需要加锁通知？
    uint64_t u = 1;
    //printf("write to eventfd %d... \n", rev->rsp_event_fd);
    int size = write(rev->rsp_event_fd, &u, sizeof(uint64_t));
    if (size != sizeof(uint64_t)) perror("write rsp_event_fd fail");
    //printf("write to eventfd %d done... \n", rev->rsp_event_fd);
    
}

void data_handler_by_worker(epoll_event *ev){
    reactor_event* rev = (reactor_event*)(ev->data.ptr);
    int cfd = rev->client_fd;
    int efd = rev->epoll_fd;
    int rspfd = rev->rsp_event_fd;
    char buf[1024];
    while (1)
    {
        bzero(buf, sizeof(buf));
        int size = recv(cfd, buf, sizeof(buf), 0);
        //连接关闭，从epoll中移除监听并关闭fd
        if(size == 0){
            //这里先 DEL 再 close，DEL操作有一定几率因为fd已经被close而报错，看着像执行语句被重排了
            printf("EPOLL_CTL_DEL fd:%d\n", cfd);
            if(epoll_ctl(efd, EPOLL_CTL_DEL, cfd, ev) != 0) handle_error("epoll del fail");
            if(epoll_ctl(efd, EPOLL_CTL_DEL, rspfd, ev) != 0) handle_error("epoll del rspfd fail");
            printf("close fd:%d\n", cfd);
            close(cfd);
            close(rspfd);
            //unbind_reactor_event(ev);//TODO：eventfd 的 epoll_event也需要释放，这里暂时不做
            return;
        }
        if(size == -1){
            int eno = errno;
            if(eno == EWOULDBLOCK || eno == EAGAIN){
                break;
            }
            handle_error("recv fail");
        }
        printf("%d<---: %s \n", cfd, buf);
        std::string msg(buf);
        rev->req_queue->push(msg);
        //printf("【push to queue】... cfd:%d, ev:%p rev:%p, rev->cfd:%d \n", cfd, ev, rev, rev->client_fd);
        pool.addQueue([=]{
            data_handler(rev);
            //一种错法：ev指针指向的值会在循环中变化，导致 data_handler 拿到的 reactor_event 始终是最后一个，客户端连接只能有一个拿到响应
            //data_handler(ev); 
        });
        //printf("【push to queue】 done... p:%p size:%d\n", rev->req_queue, rev->req_queue->size());
    }
}

void rsp_handler(epoll_event *ev){
    //printf("in rsp_handler... \n");
    reactor_event* rev = (reactor_event*)(ev->data.ptr);
    int cfd = rev->client_fd;
    int rsp_event_fd = rev->rsp_event_fd;
    int efd = rev->epoll_fd;
    while (1)
    {
        uint64_t u;
        //int ret = recv(rsp_event_fd, &u, sizeof(u), 0);//居然不能用recv，否则固定报错 ENOTSOCK
        size_t ret = read(rsp_event_fd, &u, sizeof(u));
        if(ret == -1){
            int eno = errno;
            if(eno == EWOULDBLOCK || eno == EAGAIN){
                break;
            }
        }
        if(ret != sizeof(u)) return;
        //printf("start to shift rsp data cfd:%d evfd:%d rev:%p p:%p,size:%d ...\n", cfd, rsp_event_fd, rev, &rev->rsp_queue, rev->rsp_queue->size());
        std::string msg;
        if(!rev->rsp_queue->shift_no_wait(msg)){
            printf("rsp queue is empty... \n");
            return;
        }
        ret = send(cfd, msg.c_str(), msg.size(), 0);
        if(ret == -1) perror("send to client fail");
        printf("%d--->: %s \n", cfd, msg.c_str());
        /* code */
    }
    
}

void connection_handler(epoll_event *ev){
    //接收客户端连接
    reactor_event *rev = (reactor_event*)ev->data.ptr;
    int efd = rev->epoll_fd;
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int cfd = 0;
    while (1)
    {
        cfd = accept(rev->client_fd, (sockaddr*)&addr, &addr_len);
        if(cfd == -1){
            int eno = errno;
            if(eno == EWOULDBLOCK || eno == EAGAIN){
                break;
            }
            handle_error("accept fail");
        }
        //将客户端连接也设置为非阻塞的
        int origin_flags = fcntl(cfd, F_GETFL, 0);
        fcntl(cfd, F_SETFL, origin_flags|O_NONBLOCK);
        //创建一个eventfd，提供给worker线程通知主线程请求处理完毕
        int rsp_event_fd = eventfd(0, EFD_SEMAPHORE|EFD_NONBLOCK);
        //绑定到epoll监听
        epoll_event bev;
        bev.events = EPOLLIN|EPOLLET;
        //绑定线程池worker，加快任务处理速度
        //创建一个用于通知响应的 eventfd
        SafeQueue<std::string> *req_queue = new SafeQueue<std::string>();
        SafeQueue<std::string> *rsp_queue = new SafeQueue<std::string>();
        bind_reacotr_event(&bev, efd, cfd, rsp_event_fd, data_handler_by_worker, req_queue, rsp_queue);
        int ret = epoll_ctl(efd, EPOLL_CTL_ADD, cfd, &bev);
        if(ret != 0) handle_error("epoll add client fd fail");

        //为rsp event fd绑定epoll 监听
        epoll_event rsp_ev;
        rsp_ev.events = EPOLLIN|EPOLLET;
        bind_reacotr_event(&rsp_ev, efd, cfd, rsp_event_fd, rsp_handler, req_queue, rsp_queue);
        ret = epoll_ctl(efd, EPOLL_CTL_ADD, rsp_event_fd, &rsp_ev);
        if(ret != 0) handle_error("epoll add rsp_event_fd fail");
        
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
    bind_reacotr_event(&ev, efd, sfd, 0, connection_handler, nullptr, nullptr);
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
        //step5. 挨个处理有io事件发生的fd，直接使用绑定在 reactor event 上的回调来处理
        //printf("epoll event num: %d\n", ev_num);
        //这种场景其实用边缘触发是最好的， 不然epoll一直在触发
        while (i < ev_num){
            epoll_event* ev = evs + i;
            if(ev->events & EPOLLIN){
                reactor_event* rev = (reactor_event*)ev->data.ptr;
                //printf("epoll event fd: %d, ev: %d\n", rev->fd, ev->events);
                rev->callback(ev);
            } else {
                printf("==== not EPOLLIN ==== events: %u\n",ev->events);
            }

            i++;
        }
    }
}