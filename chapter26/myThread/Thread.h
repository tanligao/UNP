#ifndef  __MYTHREAD_H_
#define __MYTHREAD_H_

#include <unistd.h>
#include <iostream>
#include <functional>

using namespace std;

class noncopyable
{
private:
	noncopyable(const noncopyable &);
	noncopyable& operator=(const noncopyable &);
public:
	noncopyable() {};
	~noncopyable() {};
};

class myThread : public noncopyable
{
public:
	typedef std::function<void()> thread_func;
	explicit myThread(const thread_func &func);
	~myThread();
	void start();
	int join();
	int detach();
	inline bool running() const
	{
		return m_running;
	}
	inline pthread_t get_tid () const
	{
		return m_tid;
	}

private:
	static void* _thread_func(void *arg);
	pthread_t m_tid;
	bool m_running;
	bool m_join;
	thread_func run_function;
	void run_in_thread();
};

#endif
