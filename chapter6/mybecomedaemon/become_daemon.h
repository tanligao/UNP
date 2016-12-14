#ifndef BECOME_DAEMON_H
#define BECOME_DAEMON_H

// becomeDaemon函数的参数flags掩码
// 依次为：
// 	切换当前工作目录
//	关闭所有fd
//	标准输入输出和错误未重定向
//	umask(0)
//	最大的文件描述符

#define BD_NO_CHDIR 		01
#define BD_NO_CLOSE_FILES 	02
#define BD_NO_REOPEN_STD_FDS	04

#define BD_NO_UMASK0		010

#define BD_MAX_CLOSE		8192

int becomeDaemon(int flags);

#endif
