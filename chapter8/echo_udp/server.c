// UDP echo服务器程序

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>


#define MAX_LINE 4096
#define SER_PORT 8888

#define oops(err_str,err_code) do { fprintf(stderr,"%s\n",err_str); \
			exit(1); } while(0)

void dg_echo(int sockfd,struct sockaddr *client_addr,socklen_t clilen)
{
	int n;
	socklen_t len;
	char mesg[MAX_LINE];
	
	for(; ;)
	{
		len = clilen;
		n = recvfrom(sockfd,mesg,MAX_LINE,0,client_addr,&len);
		write(fileno(stdout),mesg,n);
		sendto(sockfd,mesg,n,0,client_addr,len);
	}

}

int main(int argc,char *argv[])
{
	int sockfd;
	struct sockaddr_in sa_in,client_addr;
	
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if( sockfd < 0 )
		oops("socket error",1);

	bzero(&sa_in,sizeof(sa_in));
	sa_in.sin_family = AF_INET;
	sa_in.sin_addr.s_addr = htonl(INADDR_ANY);
	sa_in.sin_port = htons(SER_PORT);
	
	if( bind(sockfd,(struct sockaddr*)&sa_in,sizeof(sa_in)) == -1 )
		oops("bind error",1);

	dg_echo(sockfd,(struct sockaddr*)&client_addr,sizeof(client_addr));
	
	return EXIT_SUCCESS;
}
