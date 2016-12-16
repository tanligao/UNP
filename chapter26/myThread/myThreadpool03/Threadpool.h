#ifndef __THREADPOOL03_H_
#define __THREADPOOL03_H_

#include <pthread.h>
#include <deque>
#include <functional>
#include "../Mutex.h"

// 每一个job都是function对象
class Threadpool
{
private:
  Threadpool(const Threadpool &);
  Threadpool& operator=(const Threadpool &);
public:
  typedef std::function<void()> Task;

  Threadpool(int thread_num = 10);
  ~Threadpool();

  void stop();
  int add_task(const Task &);
  int size();
  Task take();
private:
  volatile bool m_running;
  int m_thread_num;
  pthread_t *m_threads;
  std::deque<Task> m_task_queue;
  Mutex m_mutex;
  Condition m_cond;
private:
  int create_threads();
  static void* thread_func(void *arg);
};

#endif
