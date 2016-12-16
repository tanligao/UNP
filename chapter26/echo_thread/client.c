#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../../chapter11/lib_function.h"

#define MAX_LINE 4096
#define SER_PORT "8888"

static int sockfd;
static FILE *fp;

void* copyto(void *arg)                 
{                                       
  char send[MAX_LINE];                  
  while(fgets(send,MAX_LINE,fp) != NULL)
  {                                     
    write(sockfd,send,strlen(send));    
  }                                     
                                        
  shutdown(sockfd,SHUT_WR);             
  return NULL;                          
}                                       

void str_cli(FILE *fp_arg, int sockfd_arg)
{
  char recv[MAX_LINE];
  pthread_t tid;
  sockfd = sockfd_arg;
  fp = fp_arg;

  pthread_create(&tid,NULL,copyto,NULL);
  while( read(sockfd,recv,MAX_LINE) > 0 )
  {
    fputs(recv,stdout);
  }
}

int main(int argc,char *argv[])
{
  if( argc != 2 )
  {
    oops("usage:./client <ipaddress>",1);
  }

  int sockfd = tcp_connect(argv[1],SER_PORT);
  if( sockfd == -1 )
  {
    oops("tcp connect error",1);
  }

  str_cli(stdin,sockfd);

  return EXIT_SUCCESS;
}


