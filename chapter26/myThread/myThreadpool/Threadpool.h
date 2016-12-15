#ifndef __THREADPOOL_H_
#define __THREADPOOL_H_

#include "../Thread.h"
#include "../Mutex.h"
#include <functional>
#include <vector>
#include <memory>
#include <queue>


class Threadpool final
{
public:
  Threadpool(const Threadpool &) = delete;
  Threadpool& operator= (const Threadpool &) = delete;

  typedef std::function<void()> task_func;

  explicit Threadpool(size_t _thread_num);
  ~Threadpool();

  void start();
  void stop();
  void submit_task(const task_func &);

private:
  void run_thread();
  task_func take_task();
  std::vector<std::shared_ptr<myThread>> threadpool;
  std::queue<task_func> task_queue;

  size_t m_thread_num;
  bool m_running;
  Mutex m_mutex;
  Condition m_cond;
};

#endif
