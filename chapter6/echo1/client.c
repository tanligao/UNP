// 使用IO复用的echo客户端：select

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <error.h>
#include <string.h>
#include "read_write_n.h"

#define SER_PORT 8888
#define MAX_LINE 4096

#define oops(str_msg,err_code) { perror(str_msg); exit(err_code);}

#define max(a,b) (a)>=(b)?(a):(b)

// 初始化客户端连接,返回客户端sockfd
int init_client_connect(const char *ser_ip,int ser_port)
{
	int sockfd;
	struct sockaddr_in sa_in;
	bzero(&sa_in,sizeof(sa_in));
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(ser_port);
	inet_pton(AF_INET,ser_ip,&sa_in.sin_addr);
	
	if(connect(sockfd,(struct sockaddr *)&sa_in,sizeof(sa_in)) == -1)
		return -1;	

	return sockfd;
}

// 发送标准输入到服务器，然后阻塞读取回显到标准输出上
void str_client(FILE *fp,int sockfd)
{
	fd_set read_set;
	int nfds,stdineof,n;
	char buf[MAX_LINE];
	stdineof = 0;
	FD_ZERO(&read_set);
	
	for( ; ; )
	{
		if( stdineof == 0)
			FD_SET(fileno(fp),&read_set);
		FD_SET(sockfd,&read_set);
		nfds = max(fileno(fp),sockfd) + 1;
		select(nfds,&read_set,NULL,NULL,NULL);
		if( FD_ISSET(sockfd,&read_set) )
		{
			if( (n = read(sockfd,buf,MAX_LINE)) == 0 )
			{
				if( stdineof == 1)
					return;
				else
					oops("server terminated prematurely",1);
			}
			write(fileno(stdout), buf,n);
		}
		if( FD_ISSET(fileno(fp),&read_set) )
		{
			if( (n = read(fileno(fp),buf,MAX_LINE)) == 0)
			{
				stdineof = 1;
				shutdown(sockfd,SHUT_WR);
				FD_CLR(fileno(fp), &read_set);
				continue;	
			}
			writen(sockfd,buf,n);
		}
	}
}

int main(int argc, char *argv[])
{
	if( argc != 2)
	{
		printf("usage:./client <ipaddress>\n");
		exit(1);
	}
	int sockfd = -1;
	sockfd = init_client_connect(argv[1],SER_PORT);
	if( sockfd == -1 )
		oops("init client connect error\n",1);
	
	str_client(stdin,sockfd);

	return 0;		
}

