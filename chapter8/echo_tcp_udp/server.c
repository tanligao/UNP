// 使用IO multiplexing: epoll echo服务器
// 接收使用tcp或者udp的客户端连接服务器，并回显给客户端

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>

#define MAX_LINE 4096
#define SER_PORT 8888
#define LISTENQ 1024
#define MAX_EVENT 16

#define oops(err_str,err_code) do { fprintf(stderr,"%s\n",err_str); \
	exit(err_code) ; } while(0)

// 初始化接收tcp连接的socket
int init_tcp_socket(int ser_port)
{
	int listenfd = -1;
	listenfd = socket(AF_INET,SOCK_STREAM,0);

	struct sockaddr_in sa_in;
	bzero(&sa_in,sizeof(sa_in));
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(ser_port);
	sa_in.sin_addr.s_addr = htonl(INADDR_ANY);

	if( bind(listenfd,(struct sockaddr*)&sa_in,sizeof(sa_in)) == -1 )
		oops("bind error",1);

	if( listen(listenfd,LISTENQ) == -1 )
		oops("listen error",1);

	return listenfd;		
}

// 初始化udp的socket,没有listenfd,有bind
int init_udp_socket(int ser_port)
{
	int udpfd = -1;
	udpfd = socket(AF_INET,SOCK_DGRAM,0);

	struct sockaddr_in sa_in;
	bzero(&sa_in,sizeof(sa_in));
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(ser_port);
	sa_in.sin_addr.s_addr = htonl(INADDR_ANY);

	if( bind(udpfd,(struct sockaddr*)&sa_in,sizeof(sa_in)) == -1 )
		oops("udp bind error",1);

	return udpfd;
}

void str_echo(int listenfd,int udpfd)
{
	struct epoll_event ev,ev_list[MAX_EVENT];
	int epfd,n_ready,i,connfd,len,n;
	char buf[MAX_LINE];

	epfd = epoll_create1(EPOLL_CLOEXEC);
	if( epfd == -1 )
		oops("create epoll error",1);

	ev.events = EPOLLIN;
	ev.data.fd = listenfd;
	if( epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev) == -1 )
		oops("add listenfd error",1);
	ev.events = EPOLLIN;
	ev.data.fd = udpfd;
	if( epoll_ctl(epfd,EPOLL_CTL_ADD,udpfd,&ev) == -1 )
		oops("add udpfd error",1);

	for( ; ;)
	{
		n_ready = epoll_wait(epfd,ev_list,MAX_EVENT,-1);
		if( n_ready == -1 )
		{
			if( errno == EINTR )
				continue;
			else
				oops("epoll wait error",1);
		}
		for( i = 0; i < n_ready; ++i )
		{
			if( ev_list[i].events & EPOLLIN )
			{
				if( ev_list[i].data.fd == listenfd )
				{
					connfd = accept(listenfd,NULL,NULL);
					if( connfd == -1 )
						oops("accept error",1);
					ev.events = EPOLLIN;
					ev.data.fd = connfd;
					if( epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev) == -1 )
						oops("add connfd error",1);

				}
				else if( ev_list[i].data.fd == udpfd )
				{
					int n;
					struct sockaddr_in cliaddr;
					len = sizeof(cliaddr);
					n = recvfrom(udpfd,buf,MAX_LINE,0,(struct sockaddr*)&cliaddr,&len);
					fprintf(stdout,"%s",buf);
					sendto(udpfd,buf,n,0,(struct sockaddr*)&cliaddr,len);
					memset(buf,'\0',sizeof(buf));
				}
				else
				{
					connfd = ev_list[i].data.fd;
					n = read(connfd,buf,MAX_LINE);
					if( n < 0 )
						oops("read connfd error",1);
					if( n == 0 )
					{
						fprintf(stdout,"close %d fd connection\n",connfd);
						close(connfd);
						continue;
					}
					write(connfd,buf,n);
					fprintf(stdout,"%s",buf);		
					memset(buf,'\0',sizeof(buf));
				}
			}
			else if( ev_list[i].events & (EPOLLERR | EPOLLHUP) )
			{
				if( close(ev_list[i].data.fd) == -1 )
					oops("close fd error",1);
				fprintf(stderr,"%s\n","close fd error");
			}
		}				
	}	
}

// 接收子进程发送的SIGCHLD信号
void sig_child(int sig_no)
{
	pid_t pid;
	int stat;

	// 等待所有子进程结束，输出子进程pid 	
	while( (pid = waitpid(-1,&stat,WNOHANG)) > 0 )
		printf("child %d terminated\n",pid);

	return; 
}

int main(int argc, char *argv[])
{
	int listenfd = -1, udpfd = -1;
	listenfd = init_tcp_socket(SER_PORT);
	udpfd = init_udp_socket(SER_PORT);

	if( listenfd == -1 || udpfd == -1 )
		oops("init socket error",1);

	signal(SIGCHLD,sig_child);

	str_echo(listenfd,udpfd);

	return EXIT_SUCCESS;
}

