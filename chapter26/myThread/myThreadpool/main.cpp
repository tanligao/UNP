#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include "Threadpool.h"

using namespace std;

#define oops(err_str,err_code) do { cerr << err_str << endl; \
          exit(1); } while(0)

void func(int num)
{
  cout << "thread" << num << " cur thread id: " << pthread_self() << endl;
}

int main(int argc,char *argv[])
{
  if( argc != 2 )
  {
    oops("usage:./threadpool <thread_num>",1);
  }

  int thread_num = atoi(argv[1]);
  Threadpool threads(thread_num);
  
  threads.start();

  for(int i = 0; i < thread_num; ++i)
  {
    threads.submit_task(std::bind(func,i));
  }

  sleep(5);

  threads.stop();

  return EXIT_SUCCESS;
}
