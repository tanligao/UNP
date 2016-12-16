#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "Threadpool.h"

using namespace std;

class MyTask
{  
public:  
    MyTask(){}
      
    int run(int i, const char* p)  
    {  
        printf("thread[%lu] : (%d, %s)\n", pthread_self(), i, (char*)p);  
        sleep(1);  
        return 0;  
    }  
}; 

int main()
{
  Threadpool threadpool(10);
  MyTask task_obj[20];

  for(int i = 0; i < 20; ++i)
  {
    threadpool.add_task(std::bind(&MyTask::run, &task_obj[i], i, "helloworld"));
  }

  while(1)
  {
     printf("there are still %d tasks need to process\n", threadpool.size());  
    if( threadpool.size() == 0 )
    {
      threadpool.stop();
      cout << "threadpool will exit from main" << endl;
      break;
    }
    sleep(2);
  }

  return EXIT_SUCCESS;
}
