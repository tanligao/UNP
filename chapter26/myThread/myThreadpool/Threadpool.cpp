#include "Threadpool.h"
#include <assert.h>

Threadpool::Threadpool(size_t _thread_num):
  m_thread_num(_thread_num),
  m_running(false),
  m_mutex(),
  m_cond(m_mutex)
{
  assert(m_thread_num > 0);
  threadpool.reserve(m_thread_num);
}

Threadpool::~Threadpool()
{
  if( m_running )
    stop();
}

void Threadpool::start()
{
  assert(!m_running);
  assert(m_thread_num>0);

  m_running = true;
  for(size_t i = 0; i < m_thread_num; ++i)
  {
    std::shared_ptr<myThread> thread = std::make_shared<myThread>(std::bind(&Threadpool::run_thread, this));
    threadpool.push_back(thread);
    thread->start();
  }
}

void Threadpool::stop()
{
  assert(m_running);
  m_running = false;

  m_cond.wakeAll();

  for(auto& thread : threadpool)
  {
    thread->join();
  }
}

Threadpool::task_func Threadpool::take_task()
{
  {
    MutexGuard lock(m_mutex);
    while( m_running && task_queue.empty())
    {
      m_cond.wait();
    }
  }

  MutexGuard lock(m_mutex);
  task_func task;
  if( !task_queue.empty() )
  {
    task = task_queue.front();
    task_queue.pop();
  }
  return task;
}

void Threadpool::submit_task(const task_func &_func)
{
  {
    MutexGuard lock(m_mutex);
    task_queue.push(_func);
  }
  m_cond.wake();
}

void Threadpool::run_thread()
{
  assert(m_running);
  while(m_running)
  {
    try
    {
      task_func task = take_task();
      if( task )
        task();
    }
    catch(...)
    {
      cout << "Exception on run_thread" << endl;
    }
  }
}
