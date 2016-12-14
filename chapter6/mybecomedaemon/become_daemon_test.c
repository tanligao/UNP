#include <stdio.h>
#include <stdlib.h>
#include "become_daemon.h"

#define oops(strerr,error_code) \
		fprintf(stderr,strerr);\
		exit(error_code);

int main(int argc, char* argv[])
{
	becomeDaemon(0);
	sleep(120);
	exit(EXIT_SUCCESS);
}
