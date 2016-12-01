#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <time.h>
#include <string.h>

#define	OOPS(str,err_code) \
		fprintf(stderr,str); \
		exit(err_code);

#define SER_PORT	8888
#define MAXLINE		4096

// 返回sockfd
int init_server_socket(int port,int listen_num)
{
	int sockfd = -1;
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) <0 )
		return -1;
	struct sockaddr_in sa_in;
	bzero(&sa_in,sizeof(sa_in));
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(port);
	sa_in.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(sockfd,(struct sockaddr*)&sa_in,sizeof(sa_in)) == -1)
		return -1;
	listen(sockfd,listen_num);
	return sockfd;			
}

void str_time(int listenfd)
{
	time_t cur_time;
	char str_time[MAXLINE];
	for( ; ; )
	{
		int sockfd = accept(listenfd,(struct sockaddr*)NULL,NULL);
		cur_time = time(NULL);
		snprintf(str_time,sizeof(str_time),"%.24s\r\n",ctime(&cur_time));
		write(sockfd,str_time,strlen(str_time));
		fprintf(stdout,str_time,strlen(str_time));
		close(sockfd);
	}
}

int main(int argc,char *argv[])
{
	int listenfd;
	listenfd = init_server_socket(SER_PORT,2);
	if(listenfd == -1)
	{
		printf("init socket error!\n");
		exit(1);
	}
	str_time(listenfd);
	return 0;
}
