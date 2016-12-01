#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>

#define SER_PORT	8888
#define MAXLINE		4096
/*
	得到服务器的时间，然后显示在终端
*/


// 客户端初始化socket，返回sockfd
int init_tcp_socket(int port,const char *str_ip)
{
	int sockfd;
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		return -1;
	struct sockaddr_in sa_in;
	bzero(&sa_in,sizeof(sa_in));
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(port);
	if(inet_pton(AF_INET,str_ip,&sa_in.sin_addr) <= 0)
		return -1;
	if(connect(sockfd,(struct sockaddr*)&sa_in,sizeof(sa_in)) < 0)
		return -1;
	return sockfd;
}

int main(int argc,char *argv[])
{
	if(argc != 2)
	{
		printf("usage:client <ip>\n");
		exit(1);
	}
        int n;
	char str[MAXLINE+1];
	int sockfd = init_tcp_socket(SER_PORT,argv[1]);
	printf("sockfd: %d\n",sockfd);
	if(sockfd == -1)
	{
		printf("init socket error\n");
		exit(1);
	}
        while((n = read(sockfd,str,MAXLINE)) > 0)
        {
                str[n] = '\0';
                if(fputs(str,stdout) == EOF)
                        exit(1);
        }
        if(n < 1)
                exit(1);
        close(sockfd);

	return 0;
}
