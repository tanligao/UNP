#include "Threadpool.h"

#include <iostream>
#include <assert.h>

using namespace std;

Threadpool::Threadpool(int thread_num) : 
  m_running(true),
  m_thread_num(thread_num),
  m_mutex(),
  m_cond(m_mutex)
{
  create_threads();
}

Threadpool::~Threadpool()
{
   stop();
}

void Threadpool::stop()
{
  if( !m_running )
    return;
 
  m_running = false;
  m_cond.wakeAll();

  for(int i = 0; i < m_thread_num; ++i)
  {
    pthread_join(m_threads[i],NULL);
  }

  free(m_threads);
  m_threads = NULL;
}

int Threadpool::add_task(const Task &task)
{
  m_mutex.lock();
  m_task_queue.push_back(task);
  int size = m_task_queue.size();
  m_mutex.unlock();
  m_cond.wake();
  return size;
}

int Threadpool::size()
{
  m_mutex.lock();
  int size = m_task_queue.size();
  m_mutex.unlock();
  return size;
}

Threadpool::Task Threadpool::take()
{
  Task task = NULL;
  m_mutex.lock();
  while( m_task_queue.empty() && m_running )
  {
    m_cond.wait();
  }
  if( !m_running )
  {
    m_mutex.unlock();
    return task;
  }
  assert(!m_task_queue.empty());
  task = m_task_queue.front();
  m_task_queue.pop_front();
  m_mutex.unlock();
  return task;
}

int Threadpool::create_threads()
{
  m_threads = (pthread_t*)malloc(sizeof(pthread_t)*m_thread_num);

  for(int i = 0; i < m_thread_num; ++i)
  {
    pthread_create(&m_threads[i],NULL,thread_func,this);
  }
  return 0;
}

void* Threadpool::thread_func(void *arg)
{
  pthread_t tid = pthread_self();
  Threadpool *pool = static_cast<Threadpool*>(arg);
  while(pool->m_running)
  {
    Threadpool::Task task = pool->take();
    if( !task )
    {
      cout << "thread " << tid << " will exit\n" << endl;
      break;
    }
    assert(task);
    task();
  }
  return 0;
}
