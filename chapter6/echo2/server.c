// 使用IO multiplexing的echo服务器:poll
// 而客户端使用的是select
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include "read_write_n.h"

#define MAX_LINE 4096
#define SER_PORT 8888
#define MAX_CLIENT 1024		// 最多客户端数量

#ifndef INFTIM
#define INFTIM	-1
#endif

static void oops(const char *err_str,int err_code)
{
	fprintf(stdout,"%s\n",err_str);
	exit(err_code);
}

struct pollfd clientfd[MAX_CLIENT];

int init_server_socket(int ser_port)
{
	int listenfd = -1;
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in sa_in;
	bzero(&sa_in,sizeof(sa_in));
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(ser_port);
	sa_in.sin_addr.s_addr = htonl(INADDR_ANY);

	if( bind(listenfd,(struct sockaddr*)&sa_in,sizeof(sa_in)) == -1)
		oops("bind socket error",1);

	if( listen(listenfd,MAX_CLIENT) == -1)
		oops("listen socket error",1);

	return listenfd;
}

// 监听socket,同时接收客户端消息,回显给客户端
void str_echo(int listenfd)
{
	clientfd[0].fd = listenfd;
	clientfd[0].events =POLLRDNORM;

	int n_ready;
	int n_maxfd = 0;
	int connfd,connlen;
	struct sockaddr_in clientaddr;
	int i,n_read;
	char buf[MAX_LINE];
	for( ; ; )
	{
		n_ready = poll(clientfd,n_maxfd+1,INFTIM);
		
		if( clientfd[0].revents & POLLRDNORM )
		{
			connlen = sizeof(clientaddr);
			connfd = accept(listenfd,(struct sockaddr*)&clientaddr,&connlen);
			
			for( i = 1; i < MAX_CLIENT; ++i)
			{
				if( clientfd[i].fd < 0)
					break;
			}
			if( i == MAX_CLIENT )
				oops("too many client",1);
			clientfd[i].fd = connfd;
			clientfd[i].events = POLLRDNORM;
			if( i > n_maxfd )
				n_maxfd = i;
			
			if( --n_ready <= 0)
				continue;
		}
		for( i = 1; i < MAX_CLIENT; ++i)
		{
			if( clientfd[i].fd < 0 )
				continue;
			if( clientfd[i].revents & (POLLRDNORM | POLLERR) )
			{
				if( (n_read = read(clientfd[i].fd,buf,MAX_LINE)) < 0)
				{
					if( errno == ECONNRESET )
					{
						close(clientfd[i].fd);
						clientfd[i].fd = -1;
					}
					else
					{
						oops("read error",1);
					}
					
				}
				else if( n_read == 0)
				{
					close(clientfd[i].fd);
					clientfd[i].fd = -1;
				}
				else
				{
					writen(clientfd[i].fd,buf,n_read);
					write(fileno(stdout),buf,n_read);
				}
				if( --n_ready <= 0)
					break;
			}
		}
	}


}

int main(int argc,char *argv[])
{
	int listenfd = -1,i;
	listenfd = init_server_socket(SER_PORT);
	
	if(listenfd == -1)
		oops("init server socket error",1);
	
	for( i=0; i < MAX_CLIENT; ++i)
		clientfd[i].fd = -1;

	str_echo(listenfd);

	return 0;
}
