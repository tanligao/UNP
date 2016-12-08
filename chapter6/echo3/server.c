// 使用IO复用的echo服务器: epoll
// 客户端使用select进行IO操作

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/epoll.h>
#include <strings.h>
#include "read_write_n.h"

#define SER_PORT 8888
#define MAX_LINE 4096
#define OPEN_MAX 2048
#define EVENT_MAX 16
#define LISTEN_MAX 1024

// do{ }while(0)在这里是用于防止宏定义由于\,出现莫名错误
#define oops(err_str,err_code) do { fprintf(stderr,"%s\n",err_str); \
		exit(1); } while(0)

// 初始化socket，返回监听sockfd，失败则返回-1
int init_server_socket(int ser_port)
{
	int listenfd = -1;
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in sa_in;
	bzero(&sa_in,sizeof(sa_in));
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(ser_port);
	sa_in.sin_addr.s_addr = htonl(INADDR_ANY);

	int opt = 1;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	if( bind(listenfd,(struct sockaddr*)&sa_in,sizeof(sa_in)) == -1 )
	{
		oops("bind address error",1);
	}
	if( listen(listenfd,LISTEN_MAX) == -1 )
		oops("listen error",1);

	return listenfd;
}

void str_echo(int listenfd)
{
	struct epoll_event ev;
	struct epoll_event ev_list[EVENT_MAX];

	char buf[MAX_LINE];
	int epfd,n_ready,i,connfd;
	int n_read;

	epfd = epoll_create1(EPOLL_CLOEXEC);
	if( epfd == -1 )
		oops("create epoll error",1);
	
	ev.events = EPOLLIN;
	ev.data.fd = listenfd;
	if( epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev) == -1 )
		oops("add fd to epollfd error",1);

	for( ; ; )
	{
		n_ready = epoll_wait(epfd,ev_list,OPEN_MAX,-1);
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
					if( connfd < 0 )
						oops("accept fd error",1);
					ev.events = EPOLLIN;
					ev.data.fd = connfd;
					if( epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev) == -1 )
						oops("add fd error",1);

				}
				else
				{
					connfd = ev_list[i].data.fd;
					n_read = read(connfd,buf,MAX_LINE);
					
					writen(connfd,buf,n_read);
					write(fileno(stdout),buf,n_read);
				}
			}
			else if( ev_list[i].events & (EPOLLHUP | EPOLLERR) )
			{
				if( close(ev_list[i].data.fd) == -1 )
					oops("close fd error",1);
				fprintf(stdout,"%s\n","close fd");
			}
		}
	}
	close(epfd);
}

int main(int argc,char *argv[])
{
	int listenfd = -1;
	listenfd = init_server_socket(SER_PORT);
	if( listenfd == -1 )
		oops("init socket error",1);

	str_echo(listenfd);

	return 0;	
}

