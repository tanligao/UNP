// UDP echo客户端

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>

#define MAX_LINE 4096
#define SER_PORT 8888

#define oops(err_str,err_code) do { fprintf(stderr,"%s\n",err_str); \
			exit(err_code); } while(0)

void dg_cli(FILE *fp,int sockfd,const struct sockaddr *ser_addr,socklen_t ser_len)
{
	int n;
	char send_line[MAX_LINE],recv_line[MAX_LINE];
	while( fgets(send_line,MAX_LINE,fp) != NULL )
	{
		sendto(sockfd,send_line,strlen(send_line),0,ser_addr,ser_len);
		n = recvfrom(sockfd,recv_line,MAX_LINE,0,NULL,NULL);
		recv_line[n] = 0;
		fputs(recv_line,stdout);
	}
}

int main(int argc,char *argv[])
{
	if( argc != 2 )
		oops("usage:./client <ipaddress>",1);

	int connfd;
	connfd = socket(AF_INET,SOCK_DGRAM,0);
	if( connfd < 0 )
		oops("socket error",1);
	
	struct sockaddr_in sa_in;
	bzero(&sa_in,sizeof(sa_in));

	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(SER_PORT);
	if( inet_pton(AF_INET,argv[1],&sa_in.sin_addr) == -1 )
		oops("ser_ip to number error",1);

	dg_cli(stdin,connfd,(struct sockaddr*)&sa_in,sizeof(sa_in));

	return EXIT_SUCCESS; 
}
