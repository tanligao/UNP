#include <iostream>
#include "Threadpool.h"

#define THREAD_NUM 10

using namespace std;


int main(int argc,char *argv[])
{
  char szTmp[] = "hello thread";

  myTask task_obj;
  task_obj.set_arg(szTmp);

  Threadpool threadpool(THREAD_NUM);

  for(int i = 0; i < 20; ++i)
  {
    threadpool.add_task(&task_obj);
  }
  
  while(1)
  {
    cout << "there are " << threadpool.size() << " need to process" << endl;
    if( threadpool.size() == 0 )
    {
      threadpool.stop();
      cout << "will exit from main" << endl;
      break;
    }
    sleep(2);
  }
	cout << "main thread done" << endl;
	return 0;
}
