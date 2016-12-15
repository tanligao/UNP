#ifndef  __MYMUTEX_H_
#define __MYMUTEX_H_

#include <pthread.h>
#include <time.h>
#include "Thread.h"

class Mutex : public noncopyable
{
public:
	explicit Mutex(bool isprocess_shared = false);
	~Mutex();
	int lock();
	int unlock();
	inline pthread_mutex_t* get_mutex_ptr()
	{
		return &m_mutex;
	}
private:
	pthread_mutex_t m_mutex;
	bool process_shared;
};

// use the constructor and destructor to lock and unlock
class MutexGuard : public noncopyable
{
public:
	explicit MutexGuard(Mutex &_mutex):
		m_mutex(_mutex)
	{
		m_mutex.lock();
	}
	~MutexGuard()
	{
		m_mutex.unlock();
	}
private:
	Mutex &m_mutex;
};

class Condition : public noncopyable
{
public:
	Condition(Mutex &_mutex):
		m_mutex(_mutex)
	{
		pthread_cond_init(&m_cond,NULL);
	}
	~Condition()
	{
		pthread_cond_destroy(&m_cond);
	}
	int wait()
	{
		return pthread_cond_wait(&m_cond,m_mutex.get_mutex_ptr());
	}
	int wait_for_sec(int sec)
	{
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		// current time + sec
		ts.tv_sec += sec;
		return pthread_cond_timedwait(&m_cond,m_mutex.get_mutex_ptr(),&ts);
	}
	int wake()
	{
		return pthread_cond_signal(&m_cond);
	}
	int wakeAll()
	{
		return pthread_cond_broadcast(&m_cond);
	}
private:
	pthread_cond_t m_cond;
	Mutex &m_mutex;
};

#endif