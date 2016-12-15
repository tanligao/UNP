#include "Thread.h"

#include <assert.h>
#include <iostream>

using namespace std;

myThread::myThread(const thread_func &func):
	m_running(false),
	m_join(false),
	run_function(func)
{

}

// join
myThread::~myThread()
{
	if( m_running && m_join )
		pthread_detach(m_tid);
	if( m_running )
		pthread_cancel(m_tid);
}

void* myThread::_thread_func(void *arg)
{
	myThread *run_thread = static_cast<myThread*>(arg);
	if( run_thread )
	{
		run_thread->run_in_thread();
	}
	return nullptr;
}

void myThread::start()
{
	assert(!m_running);
	m_running = true;
	if( pthread_create(&m_tid,NULL,myThread::_thread_func,this) != 0 )
	{
		cout << "create pthread error" << endl;
	}
}

int myThread::join()
{
	if( !m_join )
	{
		m_join = true;
		return pthread_join(m_tid,NULL);
	}
	return -1;
}

int myThread::detach()
{
	if( m_join && m_join)
	{
		m_join = false;
		return pthread_detach(m_tid);
	}
	return -1;
}

void myThread::run_in_thread()
{
	assert(m_running);
	try
	{
		if( run_function )
			run_function();
	}
	catch(...)
	{
		cout << "run_in_thread throw error" << endl;
	}
}

