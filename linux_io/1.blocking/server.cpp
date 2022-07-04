#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define handle_error(msg)  do { perror(msg); exit(EXIT_FAILURE); } while (0)
//作用：AF_UNIX模式下，监听的文件地址
#define MY_SOCK_PATH "./sockpath"
//作用：N connection requests will be queued before further requests are refused.
#define LISTEN_BACKLOG 2
//bind doc: https://man7.org/linux/man-pages/man2/bind.2.html
#define BUFFER_SIZE 1024

/**
 * @brief 基于 unix domain socket 的通信，因为省去了网络协议栈的一些组包解包操作，效率比基于网络的要高
 * 进程间通信（IPC）的一种很好的实现方案。
 * @return int 
 */
int ListenUnixDomain(){
     //监听socket的fd
    int sfd;
    //监听的地址描述结构
    struct sockaddr_un udf_addr;
    //创建监听socket
    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) handle_error("socket");
    //设置监听地址，此处使用 unix domain socket进行通信，MY_SOCK_PATH为文件地址
    memset(&udf_addr, 0, sizeof(udf_addr));
    udf_addr.sun_family = AF_UNIX;
    strncpy(udf_addr.sun_path, MY_SOCK_PATH, sizeof(udf_addr.sun_path) - 1);
    //将监听地址绑定到 socket fd上
    if (bind(sfd, (struct sockaddr *) &udf_addr, sizeof(udf_addr)) == -1) handle_error("bind");
    //监听 socket fd, sfd为socket的fd，LISTEN_BACKLOG为远端请求到达前，队列
    if (listen(sfd, LISTEN_BACKLOG) == -1) handle_error("listen");
    return sfd;
}

int ListenINETPort(){
	//监听的 inet 地址结构体
	struct sockaddr_in inet_addr;
	inet_addr.sin_family = AF_INET;
	inet_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_addr.sin_port = htons(8080);
	//创建监听socket，常用类型 SOCK_STREAM-tcp SOCK_DGRAM-udp SOCK_RAW-icmp，可以叠加SOCK_NONBLOCK将socket设置为非阻塞模式，如 SOCK_STREAM|SOCK_NONBLOCK
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	//inet地址绑定到socket
	if( bind(sfd, (struct sockaddr *)&inet_addr, sizeof(inet_addr)) == -1 ) handle_error("bind inet error");
	//监听，第二个参数定义积压的连接数，例如这里定义为2，那么超过2个积压后，客户端可以收到 ECONNREFUSED 错误
	if(listen(sfd, LISTEN_BACKLOG) == -1) handle_error("listen");
	return sfd;
}

int AcceptAndRecvUnixDomain(int sfd){
    while(1){
		struct sockaddr_un client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		int cfd = accept(sfd, (struct sockaddr*)&client_addr, &client_addr_len);
		if(-1 == cfd) {
			printf("accept fail:%d\n", errno);
			return 0;
		}
		printf("client connect success:%d\n", cfd);
		char buffer[BUFFER_SIZE];

		while(1) {
			memset(buffer, 0, sizeof(buffer));
			int recvbytes = recv(cfd, buffer, sizeof(buffer),0);
			if(recvbytes == 0) { 
				printf("client is disconnect.\n");
				break;
			}
			if(recvbytes < 0) {
				printf("recv err:%d.\n", errno);
				continue;
			}
			printf("client->server:%s\n", buffer);
	        // 原样返回
        	send(cfd, buffer, recvbytes, 0);
		}
		close(cfd);
	}
	close(sfd);
}

int AcceptAndRecvINET(int sfd){
	//接收连接的循环
	while(1){
		//创建接收的socket地址
		sockaddr_in client_addr;
		socklen_t client_addr_length = sizeof(client_addr);
		//接收客户端连接
		int cfd = accept(sfd, (struct sockaddr*)&client_addr, &client_addr_length);
		if(cfd == -1){
			printf("accept fail: %d", errno);
			return 0;
		}
		printf("accept connect success: %d", cfd);
		//定义一个缓冲区数据块，用于接收客户端的数据
		char buf[BUFFER_SIZE];
		//接收数据的循环
		while (1)
		{
			bzero(buf, 0);
			//接收客户端的数据，最后一个参数是flags，用于控制接收消息的模式
			int recvbytes = recv(cfd, buf, sizeof(buf), 0);
			//客户端断开
			if(recvbytes == 0){
				printf("client is disconnect: %d \n", cfd);
				break;
			}
			//接收错误，继续
			if(recvbytes < 0){
				printf("recv error: %d", errno);
				continue;
			}
			//打印结果
			printf("client->server: %s \n", buf);
			//原样返回
			send(cfd, buf, sizeof(buf), 0);
		}
		close(cfd);
	}
	close(sfd);
}

int main(){
    //监听的socket fd
    // int sfd = ListenUnixDomain();
    // AcceptAndRecvUnixDomain(sfd);
	int sfd = ListenINETPort();
	AcceptAndRecvINET(sfd);
}