#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include "become_daemon.h"

int becomeDaemon(int flags)
{
	int maxfd = -1;
	int fd;
	switch(fork())		// 成为一个后台进程，父进程exit，子进程继续运行
	{
		case -1:
			return -1;
		case 0:
			break;
		default:
			_exit(EXIT_SUCCESS);
			break;
	}
	if(setsid() == -1)	// 在新会话中运行程序
	{
		return -1;
	}
	switch(fork())		// 确保不是进程会话的首进程
	{
		case -1:
			return -1;
		case 0:
			break;
		default:
			_exit(EXIT_SUCCESS);
			break;
	}
	if(!(flags & BD_NO_UMASK0))
		umask(0);
	if(!(flags & BD_NO_CHDIR))
		chdir("/");
	if(!(flags & BD_NO_CLOSE_FILES))
	{
		maxfd = sysconf(_SC_OPEN_MAX);
		if(maxfd == -1)
			maxfd = BD_MAX_CLOSE;
		for(fd = 0; fd <=maxfd; ++fd)
			close(fd);
	}

	if(!(flags & BD_NO_REOPEN_STD_FDS))
	{
		close(STDIN_FILENO);
		fd = open("/dev/null",O_RDWR);
		if(fd != STDIN_FILENO)
			return -1;
		if(dup2(STDIN_FILENO,STDOUT_FILENO) != STDOUT_FILENO)
			return -1;
		if(dup2(STDIN_FILENO,STDERR_FILENO) != STDERR_FILENO)
			return -1;
	}
	return 0;
}
