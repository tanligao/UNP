// 使用IO复用的echo服务器：select

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <strings.h>
#include "read_write_n.h"

#define oops(str,err_code) {perror(str); exit(err_code);}

#define SER_PORT 8888
#define MAX_LINE 4096
#define LIST_ENQ 1024

// 客户socketfd，以及数组最大已用下标
int client[FD_SETSIZE];
int max_index;

int init_server_socket(int ser_port)
{
	int listenfd = -1;
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in sa_in;
	bzero(&sa_in,sizeof(sa_in));
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(ser_port);
	sa_in.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if( bind(listenfd,(struct sockaddr*)&sa_in,sizeof(sa_in)) == -1 )
		return -1;
	
	if( listen(listenfd,LIST_ENQ) < 0)
		return -1;

	return listenfd;
}

void str_echo(int listenfd)
{
	fd_set read_set,all_set;
	FD_ZERO(&read_set);
	FD_ZERO(&all_set);
	FD_SET(listenfd,&all_set);
	int n_ready,n_fds,i,n;
	n_fds = listenfd;
	char buf[MAX_LINE];
	struct sockaddr_in client_addr;
	socklen_t cli_len = sizeof(client_addr);
	max_index = -1;
	for( ; ; )
	{
		read_set = all_set;
		n_ready = select(n_fds+1,&read_set,NULL,NULL,NULL);
		
		if(FD_ISSET(listenfd,&read_set))
		{
			int connfd;
			connfd = accept(listenfd,(struct sockaddr*)&client_addr,&cli_len);
			for(i = 0; i < FD_SETSIZE; ++i)
			{
				if(client[i] == -1)
					break;
			}
			if(i == FD_SETSIZE)
				oops("too many clients",1);
			client[i] = connfd;
			FD_SET(connfd,&all_set);
			if(connfd > n_fds)
				n_fds = connfd;
			if(i > max_index)
				max_index = i;
			// 木有更多的可读的fd了
			if(--n_ready <= 0)
				continue;
		}
		for(i = 0; i <= max_index; ++i)
		{
			if(client[i] < 0)
				continue;
			int clientfd = client[i];
			if(FD_ISSET(clientfd,&read_set))
			{
				if( (n = read(clientfd,buf,MAX_LINE)) == 0 )
				{
					close(client[i]);
					FD_CLR(client[i],&all_set);
					client[i] = -1;
				}
				else
				{
					writen(clientfd,buf,n);
					write(fileno(stdout),buf,n);
				}
				if(--n_ready <= 0)
					break;	
			}
			
		}
	}	
}

int main(int argc, char *argv[])
{
	int i;
	int listenfd = init_server_socket(SER_PORT);
	
	if(listenfd == -1)
		oops("init error",1);

	for(i = 0; i < FD_SETSIZE; ++i)
		client[i] = -1;

	str_echo(listenfd);	
	
	return 0;
}

