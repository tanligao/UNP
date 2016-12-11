// 使用IO复用的echo客户端：select
// 使用非阻塞进行I/O操作
// 使用非阻塞连接服务器

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#define SER_PORT 8888
#define MAX_LINE 4096

#define oops(str_msg,err_code) { perror(str_msg); exit(err_code);}

#define max(a,b) (a)>=(b)?(a):(b)

char* gf_time()
{
	struct timeval tv;
	static char str[30];
	if( gettimeofday(&tv,NULL) < 0 )
		oops("gettimeofday error",1);
	char *ptr = ctime(&tv.tv_sec);
	strcpy(str,&ptr[11]);
	snprintf(str + 8,sizeof(str) - 8,".%06ld",tv.tv_usec);

	return str;
}
// 使用非阻塞连接服务器，成功则放回0，否则-1
int connect_nonb(int sockfd,const struct sockaddr *sa_in,socklen_t sa_len,int nsec)
{
	int flags,n,error;
	socklen_t len;
	fd_set rset,wset;
	struct timeval tval;
	
	flags = fcntl(sockfd,F_GETFL,0);
	fcntl(sockfd,F_SETFL,flags | O_NONBLOCK);
	
	error = 0;
	if( (n = connect(sockfd,sa_in,sa_len)) < 0 )
	{
		if( errno != EINPROGRESS )
			return -1;
	} 
	if( n == 0 )
		goto done;		// 成功连接则直接跳到done
	FD_ZERO(&rset);
	FD_SET(sockfd,&rset);

	wset = rset;
	tval.tv_sec = nsec;
	tval.tv_usec = 0;
	if( (n = select(sockfd + 1,&rset,&wset,NULL,nsec?&tval:NULL)) == 0 )
	{
		close(sockfd);
		errno = ETIMEDOUT;
		return -1;
	}
	if( FD_ISSET(sockfd,&rset) || FD_ISSET(sockfd,&wset) )
	{
		len = sizeof(error);
		if( getsockopt(sockfd,SOL_SOCKET,SO_ERROR,&error,&len) < 0 )
			return -1;
	}
	else
	{
		oops("select error",1);
	}
done:
	fcntl(sockfd,F_SETFL,flags);
	if( error )
	{
		close(sockfd);
		errno = error;
		return -1;
	}
	return 0;
}

// 发送标准输入到服务器，然后非阻塞读取回显到标准输出上
// 每次都需要记录在发送和接收缓存的位置，便于下次继续发送数据
// 尤其适合大字节流的数据
void str_client(FILE *fp,int sockfd)
{
	int maxfdp1,val,stdineof;
	int n,nwritten;
	fd_set rset,wset;
	char to[MAX_LINE],fr[MAX_LINE];
	char *toiptr,*tooptr,*friptr,*froptr;
	toiptr = tooptr = to;
	friptr = froptr = fr;
				
	val = fcntl(sockfd,F_GETFL,0);
	fcntl(sockfd,F_SETFL,val | O_NONBLOCK);

	val = fcntl(STDIN_FILENO,F_GETFL,0);
	fcntl(STDOUT_FILENO,F_SETFL,val | O_NONBLOCK);

	val = fcntl(STDOUT_FILENO,F_GETFL,0);
	fcntl(STDOUT_FILENO,F_SETFL,val | O_NONBLOCK);

	stdineof = 0;
	maxfdp1 = max(max(STDIN_FILENO,STDOUT_FILENO),sockfd) + 1;
	for( ; ; )
	{
		FD_ZERO(&rset);
		FD_ZERO(&wset);

		if( stdineof == 0 && toiptr < &to[MAX_LINE] )
			FD_SET(STDIN_FILENO,&rset);

		if( friptr < &fr[MAX_LINE] )
			FD_SET(sockfd, &rset);

		if( tooptr != toiptr )
			FD_SET(sockfd,&wset);

		if( froptr != friptr )
			FD_SET(STDOUT_FILENO,&wset);

		select(maxfdp1,&rset,&wset,NULL,NULL);
		if( FD_ISSET(STDIN_FILENO,&rset) )
		{
			if( (n = read(STDIN_FILENO,toiptr,&to[MAX_LINE]-toiptr)) < 0 )
			{
				if( errno != EWOULDBLOCK )
					oops("read eeor on stdin",1);
			}
			else if( n == 0 )
			{
				fprintf(stderr, "%s: EOF on stdin\n",gf_time());
				stdineof = 1;
				if( tooptr == toiptr )
					shutdown(sockfd,SHUT_WR);
			}
			else
			{
				fprintf(stderr,"%s: read %d bytes from stdin\n",gf_time(),n);
				toiptr += n;
				FD_SET(sockfd,&wset);		// 用于下面发送数据
			}
		}
		if( FD_ISSET(sockfd,&rset) )
		{
			if( (n = read(sockfd,friptr,&fr[MAX_LINE] - friptr)) < 0 )
			{
				if( errno != EWOULDBLOCK )
					oops("read error on socket",1);
			}
			else if( n == 0)
			{
				fprintf(stderr,"%s: EOF on socket\n",gf_time());
				if( stdineof )
					return;
				else
					oops("str_client: server terminated prematurely",1);
			}
			else
			{
				fprintf(stderr,"%s: read %d bytes from socket\n",gf_time(),n);
				friptr += n;
				FD_SET(STDOUT_FILENO,&wset); 
			}
		}
		if( FD_ISSET(STDOUT_FILENO,&wset) && ( (n = friptr - froptr) > 0) )
		{
			if( (nwritten = write(STDOUT_FILENO,froptr,n) ) < 0 )
			{
				if( errno != EWOULDBLOCK )
					oops("write error to stdout",1);
			}
			else
			{
				fprintf(stderr,"%s: wrote %d bytes to stdout\n",gf_time(),nwritten);
				froptr += nwritten;
				if( froptr == friptr )
					froptr = friptr = fr;
			}
		}
		if( FD_ISSET(sockfd,&wset) && ( (n = toiptr - tooptr) > 0) )
		{
			if( (nwritten = write(sockfd,tooptr,n)) < 0 )
			{
				if( errno != EWOULDBLOCK )
					oops("write error to sockfd",1);
			}
			else
			{
				fprintf(stderr,"%s: wrote %d bytes to socket\n",gf_time(),nwritten);
				if( tooptr == toiptr )
				{
					toiptr = tooptr = to;
					if( stdineof )
						shutdown(sockfd,SHUT_WR);
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	if( argc != 2)
	{
		printf("usage:./client <ipaddress>\n");
		exit(1);
	}
	int sockfd = -1;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in sa_in;
	bzero(&sa_in,sizeof(sa_in));
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(SER_PORT);
	if( inet_pton(AF_INET,argv[1],&sa_in.sin_addr) < 0 )
		oops("inet_pton error",1);

	if( connect_nonb(sockfd,(struct sockaddr*)&sa_in,sizeof(sa_in),0) < 0 )
		oops("connect_nonb error",1);
	
	str_client(stdin,sockfd);

	return 0;		
}
