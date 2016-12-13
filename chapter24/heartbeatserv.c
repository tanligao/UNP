// 服务器的接收客户端发送心跳的计数

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>

#define oops(err_str,err_code) do { fprintf(stderr,"%s\n",err_str); \
	exit(err_code); } while(0)

static int	servfd;		// 客户端的scoket
static int 	nsec;		// 心跳间隔秒数
static int	maxnalarms;	// 客户端最大心跳次数
static int	nprobes;	// 自上次心跳未得到心跳的计数

static void sig_urg(int);
static void sig_alrm(int);

void heartbeat_serv(int servfd_arg,int nsec_arg,int maxnalarms_arg)
{
	servfd = servfd_arg;
	if( (nsec = nsec_arg) < 1 )
	{
		nsec = 1;
	}
	if( (maxnalarms = maxnalarms_arg) < nsec )
	{
		maxnalarms = nsec;
	}
	signal(SIGURG,sig_urg);
	fcntl(servfd,F_SETOWN,getpid());

	signal(SIGALRM,sig_alrm);
	alarm(nsec);
}

static void sig_urg(int signo)
{
	int n;
	char c;

	if( (n = recv(servfd,&c,1,MSG_OOB)) < 0)
	{
		if( errno != EWOULDBLOCK )
			oops("recv urg error",1);
	}
	send(servfd,&c,1,MSG_OOB);
	nprobes = 0;

	return;
}

static void sig_alrm(int signo)
{
	if( ++nprobes > maxnalarms )
	{
		printf("no probes from client\n");
		close(servfd);	
	}
	alarm(nsec);
	return;
}
