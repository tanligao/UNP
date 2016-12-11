// 用于生成第一个命令行参数的行数的文本文件

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc,char *argv[])
{
	if( argc != 2 )
	{
		fprintf(stderr,"%s\n","usage:./create_line <line num>");
		exit(1);
	}
	int lines = atoi(argv[1]);
	int i = 0;
	char file_str[20];
	snprintf(file_str,sizeof(file_str),"%d.lines",lines);
	int fd = open(file_str,O_CREAT | O_WRONLY);
	if( fd < 0 )
	{
		fprintf(stderr,"%s\n","open fd error");
		exit(1);
	}
	const char *test_str = "test test test test test1111111111111111111111111\n";
	for( i = 0; i < lines; ++i )
	{
		write(fd,test_str,strlen(test_str));
	}
	return EXIT_SUCCESS;
} 
