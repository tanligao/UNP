#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "read_write_n.h"

#define SER_PORT	8888
#define MAXLINE		4096

static void oops(const char *str_err,int err_code)
{
	printf("%s\n",str_err);
	exit(err_code);	
}

int init_server_socket(int port,int listen_num)
{
	int listenfd = -1;
	int b_reuse_port = 1;
	int b_reuse_addr = 1;
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	if(listenfd < 0)
		return -1;
	struct sockaddr_in sa_in;
	bzero(&sa_in,sizeof(sa_in));
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(port);
	sa_in.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(( bind(listenfd,(struct sockaddr*)&sa_in,sizeof(sa_in)) ) == -1)
		return -1;
	
	setsockopt(listenfd,IPPROTO_TCP,SO_REUSEPORT,&b_reuse_port,sizeof(b_reuse_port));
	setsockopt(listenfd,IPPROTO_TCP,SO_REUSEADDR,&b_reuse_addr,sizeof(b_reuse_addr));

	if(( listen(listenfd,listen_num) ) < 0)
		return -1;
	
	return listenfd;		
}

void str_echo(int sockfd)
{
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(struct sockaddr_in);
	char buf[MAXLINE+1];
	ssize_t n;
again:
	while(( n = recvfrom(sockfd,buf,MAXLINE,0,(struct sockaddr*)&client_addr,&addr_len) ) > 0)
	{
		char strptr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET,&client_addr.sin_addr,strptr,sizeof(strptr));
		writen(sockfd,buf,n);
		printf("recv from %s: %s",strptr,buf);
		memset(buf,'\0',sizeof(buf));
	}
	if(n < 0 && errno == EINTR)
		goto again;
	else if(n < 0)
		oops("str_echo: read error",1);	
}

void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	// 等待所有子进程发送SIGCHLD信号
	while( (pid = waitpid(-1,&stat,WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
}

int main(int argc,char *argv[])
{
	int listenfd = -1;
	struct sockaddr client_add;
	int sa_len = sizeof(struct sockaddr);
	listenfd = init_server_socket(SER_PORT,10);
	if(listenfd == -1)
		oops("init server socket error!\n",1);
	
	int sockfd,status,childfd;
        	signal(SIGCHLD,sig_chld);
        	for( ; ;)
        	{
                	sockfd = accept(listenfd,&client_add,&sa_len);
                	if(( childfd = fork()) == 0)
                	{
                        		close(listenfd);
    			str_echo(sockfd);
			exit(0);
                	}
		close(sockfd);
        	}
	return 0;	
}
