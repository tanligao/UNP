#ifndef READ_WRITE_NUM
#define READ_WRITE_NUM

#include <stdio.h>
#include <string.h>
#include <errno.h>

// read n char for vptr 
ssize_t readn(int fd,void *vptr,size_t n)
{
	size_t nread;
	size_t nleft;
	nread = 0;
	nleft = n;
	char *ptr = vptr;
	while(nleft > 0)
	{
		if((nread = read(fd,ptr,nleft)) < 0)
		{
			if(errno == EINTR)
				continue;
			else
				return -1;
		}
		else if(nread == 0)
		{
			break;
		}
		nleft -= nread;
		ptr += nread;
	}
	return (n - nleft);
}

// write n char for vptr
ssize_t writen(int fd,void *vptr,size_t n)
{
	size_t nwrite;
	size_t nleft;
	nwrite = 0;
	nleft = n;
	const char *ptr = vptr;
	while(nleft > 0)
	{
		if((nwrite = write(fd,ptr,nleft)) <= 0)
		{
			if(nwrite < 0 && errno == EINTR)
				continue;
			else
				return -1;
		}
		nleft -= nwrite;
		ptr += nwrite;
	}
	return (n - nleft);
}

#endif
