// 使用IO复用的echo客户端:select
// 服务器使用epoll接收客户端连接和数据回显

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/select.h>
#include <strings.h>
#include "read_write_n.h"
#include <string.h>

#define MAX_LINE 4096
#define SER_PORT 8888

static void oops(const char *err_str, int err_code)
{
	fprintf(stderr,"%s\n",err_str);
	exit(err_code);
}

static void err_exit(const char *err_str,int err_code)
{
	perror(err_str);
	exit(err_code);
}

// 初始化socket，连接服务器，返回成功连接的socketfd
int init_socket(const char *ser_ip,int ser_port)
{
	int sockfd = -1;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in sa_in;
	bzero(&sa_in,sizeof(sa_in));
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(ser_port);
	
	if( inet_pton(AF_INET,ser_ip,&sa_in.sin_addr) == -1 )
		oops("inet pton error",1);

	if( connect(sockfd,(struct sockaddr*)&sa_in,sizeof(sa_in)) == -1)
		oops("connect server error",1);
	
	return sockfd;
}

// 使用select监听socketfd和fileno(fp)的IO状态，进行相应的IO
// 操作
void str_cli(int connfd,FILE *fp)
{
	int n_fds = 0,n_read;
	int stdineof = 0;
	char buf[MAX_LINE];

	fd_set read_set;
	FD_ZERO(&read_set);	

	for( ; ; )
	{
		if( stdineof == 0)
			FD_SET(fileno(fp),&read_set);
		FD_SET(connfd,&read_set);
		
		n_fds = (fileno(fp) > connfd ? fileno(fp) : connfd) + 1;
		
		select(n_fds,&read_set,NULL,NULL,NULL);
				
		if( FD_ISSET(connfd,&read_set) )
		{
			if( (n_read = read(connfd,buf,MAX_LINE)) == 0)
			{
				if( stdineof == 1)
					return;
				else
					oops("read connfd error",1);
			}
			write(fileno(stdout),buf,n_read);
			memset(buf,'\0',sizeof(buf));
		}

		if( FD_ISSET(fileno(fp),&read_set) )
		{
			if( (n_read = read(fileno(fp),buf,MAX_LINE)) == 0 )
			{
				stdineof = 1;
				shutdown(connfd,SHUT_WR);
				FD_CLR(fileno(fp),&read_set);
				continue;
			}
			writen(connfd,buf,n_read);
			memset(buf,'\0',sizeof(buf));
		}
	}	
}

int main(int argc, char *argv[])
{
	if( argc != 2)
	{
		oops("usage:./client <ipaddress>",1);
	}
	int connfd = -1;
	connfd = init_socket(argv[1],SER_PORT);
	if( connfd == -1 )
		oops("init socket error",1);

	str_cli(connfd,stdin);

	return 0;
}
