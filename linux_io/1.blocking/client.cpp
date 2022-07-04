#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define MY_SOCK_PATH "./sockpath"
#define BUFFER_SIZE 1024

int ConnectUnixDomain(){
	//创建socket
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    printf("fd:%d\n", fd);
    //创建udf地址结构体
	struct sockaddr_un udf_addr;
    bzero(&udf_addr, sizeof(udf_addr));
    udf_addr.sun_family = AF_UNIX;
    strncpy(udf_addr.sun_path, MY_SOCK_PATH, sizeof(udf_addr.sun_path) - 1);
    //连接
	printf("client start connect ···\n");
	int ret = connect(fd, (struct sockaddr*)&udf_addr, sizeof(udf_addr));
	printf("client start connect ret: %d\n", ret);
	if (ret < 0) {
		printf("serv connect fail:%d\n", errno);
		return 0;
	}
	if (ret != 0) {
		printf("serv connect not success:%d\n", ret);
		return 0;
	}
	printf("serv connect success.\n");

	char sendbuf[BUFFER_SIZE] = "hello";
	char recvbuf[BUFFER_SIZE];
	while(1) {
		send(fd, sendbuf, strlen(sendbuf),0); // 发送
		printf("client->server:%s\n", sendbuf);

		memset(&recvbuf, 0, sizeof(recvbuf));
		recv(fd, recvbuf, sizeof(recvbuf),0); //接收
		printf("server->client:%s\n\n", recvbuf);
		sleep(10);
	}
	return 0;
}

int ConnectINET(){
	printf("client start create sockaddr ···\n");
	//创建待连接的socket地址
	struct sockaddr_in inet_addr;
	inet_addr.sin_family = AF_INET;
	inet_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_addr.sin_port = htons(8080);
	printf("client start create socket ···\n");
	//创建客户端socket
	int cfd = socket(AF_INET, SOCK_STREAM, 0);
	//连接
	printf("client start connect ···\n");
	int ret = connect(cfd, (struct sockaddr *)&inet_addr, sizeof(inet_addr));
	printf("client start connect ret: %d\n", ret);
	if (ret < 0) {
		printf("serv connect fail:%d\n", errno);
		return 0;
	}
	if (ret != 0) {
		printf("serv connect not success:%d\n", ret);
		return 0;
	}
	printf("client connect success: %d \n", cfd);
	char send_buf[BUFFER_SIZE] = "hello", recv_buf[BUFFER_SIZE];
	while (1)
	{
		int ret = send(cfd, send_buf, sizeof(send_buf), 0);
		if(ret < 0){
			printf("client send error: %d \n", errno);
			sleep(10);
			continue;
		}
		printf("client->server:%s\n", send_buf);
		bzero(recv_buf, 0);
		ret = recv(cfd, recv_buf, sizeof(recv_buf), 0);
		if(ret < 0){
			printf("client recv error: %d \n", errno);
			sleep(10);
			continue;
		} else if(ret == 0){
			printf("client recv disconnect\n");
			break;
		}
		printf("server->client:%s\n\n", recv_buf);
		sleep(10);
	}
	
}

int main(){
    //ConnectUnixDomain();
	ConnectINET();
}