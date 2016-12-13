// 客户端心搏函数及相关定义

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>

static int	servfd;		// 服务器fd
static int	nsec;		// 心跳间隔时间
static int	maxnprobes;	// 最大心跳次数
static int	nprobes;	// 上次服务器回复后发出的心跳次数

static void sig_urg(int);
static void sig_alrm(int);

void heartbeat_cli(int servfd_arg,int nsec_arg,int maxnprobes_arg)
{
	servfd = servfd_arg;
	if( (nsec = nsec_arg) < 1)
		nsec = 1;
	if( (maxnprobes = maxnprobes_arg) < 1 )
		maxnprobes = nsec;

	nprobes = 0;
	signal(SIGURG,sig_urg);
	fcntl(servfd,F_SETOWN,getpid());
	signal(SIGALRM,sig_alrm);
	alarm(nsec);
}

static void sig_urg(int signo)
{
	int n;
	char c;
	
	if( recv(servfd,&c,1,MSG_OOB) < 0 )
	{
		if( errno != EWOULDBLOCK )
			oops("recv msg_oob error",1);
	}
	nprobes = 0;
	return;
}

static void sig_alrm(int signo)
{
	if( ++nprobes > maxnprobes )
	{
		printf("server is unreachable\n");
		close(servfd);
		exit(0);
	}
	send(servfd,"1",1,MSG_OOB);
	alarm(nsec);
	return;
}
