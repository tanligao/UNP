// 一些网络编程的协助函数，后面可借用

#ifndef __LIB_FUNCTION_H_
#define __LIB_FUNCTION_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LISTENQ 1024

#define oops(err_str,err_code) do { fprintf(stderr,"%s\n",err_str); \
	exit(err_code); } while(0)

// 获取服务器信息，host可为ip地址或者主机名字，ser为端口或者service name
struct addrinfo* host_serv(const char *host,const char *serv, int family,int socktype);

// tcp服务器监听
int tcp_listen(const char *hostname,const char *service,socklen_t *addrlenp);

// tcp连接
int tcp_connect(const char *host,const char *serv);

// udp客户端
int udp_client(const char *host,const char *serv,struct sockaddr **saptr,socklen_t *lenp);

// udp连接
int udp_connect(const char *hostname,const char *service);

// udp服务
int udp_server(const char *hostname,const char *service,socklen_t *lenptr);


#endif

