#ifndef READ_WRITE_N
#define READ_WRITE_N

// 读取指定长度的字符串
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

ssize_t readn(int fd,char *read_str, ssize_t read_len)
{
	char *ptr = read_str;
	ssize_t n_read,n_left;
	n_read = 0;
	n_left = read_len;
	while( n_left > 0 )
	{
		if( (n_read = read(fd,ptr,n_left)) < 0)
		{
			if(errno == EINTR)
				continue;
			else
				return -1;
		}
		else if( n_read == 0)
		{
			break;
		}
		n_left -= n_read;
		ptr += n_read;
	}
	return read_len - n_left;
}

ssize_t writen(int fd,char *write_str, ssize_t write_len)
{
	ssize_t n_write,n_left;
	n_write = 0;
	n_left = write_len;
	char *ptr = write_str;
	while( n_left > 0)
	{
		if( (n_write = write(fd,ptr,n_left)) < 0)
		{
			if( errno == EINTR)
				continue;
			else
				return -1;
		}
		else if( n_write == 0 )
		{
			break;
		}
		n_left -= n_write;
		ptr += n_write;
	}
	return write_len - n_left;
}

#endif
