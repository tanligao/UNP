#include "lib_function.h"

struct addrinfo* host_serv(const char *host,const char *serv,int family,int socktype)
{
	int n;
	struct addrinfo hints,*res;

	bzero(&hints,sizeof(hints));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = family;
	hints.ai_socktype = socktype;

	if( (n = getaddrinfo(host,serv,&hints,&res)) != 0 )
		return NULL;
	return res;
}

int tcp_listen(const char *hostname,const char *service,socklen_t *addrlenp)
{
	int listenfd,n;
	const int on = 1;
	struct addrinfo hints,*res,*ressave;
	
	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	if( (n = getaddrinfo(hostname,service,&hints,&res)) != 0 )
		oops("getaddrinfo error",1);
	
	ressave = res;
	
	do {
		listenfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if( listenfd < 0 )
			continue;
		setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
		if( bind(listenfd,res->ai_addr,res->ai_addrlen) == 0 )
			break;
		close(listenfd);
	} while( (res = res->ai_next) != NULL);

	if( res == NULL)
		oops("tcp_listen error",1);
	listen(listenfd,LISTENQ);
	if( addrlenp )
		*addrlenp = res->ai_addrlen;

	freeaddrinfo(ressave);

	return listenfd;
}

int tcp_connect(const char *host,const char *serv)
{
	int sockfd,n;
	struct addrinfo hints,*res,*ressave;
	bzero(&hints,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	if( (n = getaddrinfo(host,serv,&hints,&res)) != 0 )
		oops("getaddrinfo error",1);

	ressave = res;
	
	do {
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if( sockfd < 0 )
			continue;
		if( connect(sockfd,res->ai_addr,res->ai_addrlen) == 0 )
			break;		// 成功则退出
		close(sockfd);
	} while( (res = res->ai_next) != NULL );

	if( res == NULL )
		oops("tcp_connect error",1);
	freeaddrinfo(ressave);

	return sockfd;
}

int udp_client(const char *host,const char *service,struct sockaddr **saptr,socklen_t *lenp)
{
	int sockfd,n;
	struct addrinfo hints,*res,*ressave;
	
	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	if( (n = getaddrinfo(host,service,&hints,&res)) != 0 )
		oops("getaddrinfo error",1);
	
	ressave = res;
	
	do {
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if( sockfd >= 0)
			break;
	} while( (res = res->ai_next) != NULL );

	if( res == NULL )
		oops("udp_client error",1);

	*saptr = malloc(res->ai_addrlen);
	memcpy(saptr,res->ai_addr,res->ai_addrlen);
	freeaddrinfo(res);

	return sockfd;
}

int udp_connect(const char *hostname,const char *service)
{	
	int sockfd,n;
	struct addrinfo hints,*res,*ressave;
	bzero(&hints,sizeof(hints));
	
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	if( (n = getaddrinfo(hostname,service,&hints,&res)) != 0 )
		oops("udp_connect error",1);

	ressave = res;
	
	do {
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if( sockfd < 0)
			continue;
		if( connect(sockfd,res->ai_addr,res->ai_addrlen ) == 0 )
			break;
		close(sockfd);

	} while( (res = res->ai_next) != NULL);	
	if( res == NULL )
		oops("udp_connect error",1);
	freeaddrinfo(ressave);
	return sockfd;
}

int udp_server(const char *hostname,const char *service,socklen_t *lenptr)
{
	int sockfd,n;
	struct addrinfo hints,*res,*ressave;
	
	bzero(&hints,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	
	if( (n = getaddrinfo(hostname,service,&hints,&res)) != 0 )
		oops("udp_server error",1);
	ressave = res;

	do{
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if( sockfd < 0 )
			continue;
		if( bind(sockfd,res->ai_addr,res->ai_addrlen) == 0 )
			break;
		close(sockfd);
	} while( (res = res->ai_next) != NULL );

	if( res == NULL )
		oops("udp_server error",1);
	if( lenptr )
		*lenptr = res->ai_addrlen;
	freeaddrinfo(ressave);
	return sockfd;
}
