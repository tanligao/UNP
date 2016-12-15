#ifndef __THREADPOOL98_H_
#define __THREADPOOL98_H_

#include <string>
#include <iostream>
#include <deque>
#include "../Thread.h"
#include "../Mutex.h"

using namespace std;

// 所有的job都继承该类
class Task
{
public:
  Task(void *arg = NULL,string task_name = ""):
    m_arg(arg),m_task_name(task_name)
   {

   }
  virtual ~Task() { }
  inline void set_arg(void *arg) { m_arg = arg; }

  virtual int run() = 0; 

protected:
  void *m_arg;
  string m_task_name;
};

class myTask : public Task
{
  public:
    myTask(){}
    virtual int run()
    {
      cout << "thread" << pthread_self() << " : " << (char*)m_arg << endl;
      sleep(2);
      return 0;
    }
};

class Threadpool : public noncopyable
{
 public:
  Threadpool(int thread_num = 10);
  ~Threadpool();
  void stop();
  int size();
  Task* take();
  size_t add_task(Task *task);
 private:
  int create_threads();
  static void* thread_func(void *thread_data);

 private:
  volatile bool m_running;
  int m_thread_num;
  pthread_t *m_threads;

  std::deque<Task*> m_task_queue;
  Mutex m_mutex;
  Condition m_cond;
 
};

#endif
