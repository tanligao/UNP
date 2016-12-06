// 使用IO multiplexing:select的echo客户端
// 服务器端采用poll接收客户端的连接

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <strings.h>
#include "read_write_n.h"

#define MAX_LINE 4096
#define SER_PORT 8888

// 输出错误,并结束进程
static void oops(const char *err_str,int err_code)
{
	fprintf(stderr,"%s\n",err_str);
	exit(err_code);
}

int init_client_socket(const char *ser_ip,int ser_port)
{
	int clientfd = -1;
	clientfd = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in sa_in;
	bzero(&sa_in,sizeof(sa_in));
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(ser_port);
	
	if( inet_pton(AF_INET,&sa_in.sin_addr.s_addr,ser_ip) < 0)
	{
		oops("inet_pton error",1);
		return -1;
	}

	if( connect(clientfd,(struct sockaddr*)&sa_in,sizeof(sa_in)) == -1)
	{
		oops("connect error",1);
		return -1;
	}

	return clientfd;	
}

void str_cli(FILE *fp,int clientfd)
{
	fd_set read_set;
	int stdineof = 0; 		// 标识是否输入截止
	int max_fd;
	char buf[MAX_LINE];
	int n_read;
	for( ; ; )
	{
		if( stdineof == 0 )
			FD_SET(fileno(fp),&read_set);
		FD_SET(clientfd,&read_set);
		max_fd = fileno(fp) > clientfd ? fileno(fp) : clientfd;
		
		select(max_fd + 1,&read_set,NULL,NULL,NULL);
		
		if( FD_ISSET(clientfd,&read_set) )
		{
			if( (n_read = read(clientfd,buf,MAX_LINE)) == 0)
			{
				if( stdineof == 1)
					return;
				else
					oops("server terminated prematurely",1);	
			}			
			write(fileno(stdout),buf,n_read);
		}

		if( FD_ISSET(fileno(fp),&read_set) )
		{
			if( (n_read = read(fileno(fp),buf,MAX_LINE)) == 0)
			{
				stdineof = 1;
				shutdown(clientfd,SHUT_WR);
				FD_CLR(fileno(fp), &read_set);
				continue;
			}
			writen(clientfd,buf,n_read);
		}
	}
}

int main(int argc, char *argv[])
{
	if( argc != 2 )
	{
		oops("usage:./client <ipaddress>",1);
	}
	int listenfd = -1;
	listenfd = init_client_socket(argv[1],SER_PORT);
	if( listenfd == -1)
	{
		oops("init client socket error",1);
	}
		
	str_cli(stdin,listenfd);

	return 0;
}
