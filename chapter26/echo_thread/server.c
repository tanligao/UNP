#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#include "../../chapter11/lib_function.h"

#define MAX_LINE 4096
#define SER_PORT "8888"

void str_echo(int sockfd)
{
  struct sockaddr_in client_addr;
  socklen_t addr_len = sizeof(struct sockaddr_in);
  char buf[MAX_LINE+1];
  ssize_t n;
again:
  while(( n = recvfrom(sockfd,buf,MAX_LINE,0,(struct sockaddr*)&client_addr,&addr_len) ) > 0)
  {
    char strptr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,&client_addr.sin_addr,strptr,sizeof(strptr));
    write(sockfd,buf,n);
    printf("recv from %s: %s",strptr,buf);
    memset(buf,'\0',sizeof(buf));
  }
  if(n < 0 && errno == EINTR)
    goto again;
  else if(n < 0)
    oops("str_echo: read error",1); 
}

static void* doit(void *arg)
{
  int connfd;
  connfd = *(int*)arg;
  free(arg);

  pthread_detach(pthread_self());
  str_echo(connfd);
  close(connfd);
  return NULL;
}

int main(int argc,char *argv)
{
  socklen_t addrlen,len;
  int listenfd = tcp_listen(NULL,SER_PORT,&addrlen);
  if( listenfd == -1 )
  {
    oops("tcp listen error",1);
  }
  int *iptr;
  pthread_t tid;
  struct sockaddr *cliaddr;

  cliaddr = malloc(addrlen);
  for( ; ; )
  {
    len = addrlen;
    iptr = malloc(sizeof(int));
    *iptr = accept(listenfd,cliaddr,&len);
    pthread_create(&tid,NULL,&doit,iptr);
  }
  free(cliaddr);

  return EXIT_SUCCESS;
}
