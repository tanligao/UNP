#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include "read_write_n.h"

#define SER_PORT        8888
#define MAXLINE         4096

#define oops(str,err_code) \
		fprintf(stderr,str); \
		exit(err_code);

/*
	echo: send string to server,then server
	      will send same string
*/

int init_tcp_socket(int port,const char *str_ip)
{
	int sockfd = -1;
	if(( sockfd = socket(AF_INET,SOCK_STREAM,0) ) < 0)
		return -1;
	struct sockaddr_in sa_in;
	bzero(&sa_in,sizeof(sa_in));
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(port);
	if(( inet_pton(AF_INET,str_ip,&sa_in.sin_addr) ) <= 0)
		return -1;
	if(( connect(sockfd,(struct sockaddr*)&sa_in,sizeof(sa_in)) ) < 0)
		return -1;
	return sockfd;
}

void str_cli(FILE *fp,int sockfd)
{
	char send_str[MAXLINE];
	char recv_str[MAXLINE];
	while(fgets(send_str,MAXLINE,fp) != NULL)
	{
		ssize_t write_size = writen(sockfd,send_str,strlen(send_str));
		readn(sockfd,recv_str,write_size);
		
		fputs(recv_str,stdout);
		memset(send_str,'\0',sizeof(send_str));
		memset(recv_str,'\0',sizeof(recv_str));
	}
}

int main(int argc,char *argv[])
{
	if(argc != 2)
	{
		printf("usage:./client <ipaddress>\n");
		exit(1);
	}
	int sockfd = -1;
	sockfd = init_tcp_socket(SER_PORT,argv[1]);
	if(sockfd < 0)
	{
		printf("socket init error.\n");
		exit(1);
	}
	str_cli(stdin,sockfd);
	return 0;
}


