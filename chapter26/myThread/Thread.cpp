#include "Thread.h"

#include <iostream>
#include <pthread.h>

static void* runThread(void *arg)
{
	return ((Thread*)arg)->run();
}

Thread::Thread():
	m_started(false),m_joined(false)
{
		
}

Thread::~Thread()
{
	if( m_started == 1 && m_joined == 1)
		pthread_detach(m_thread_id);
	if( m_started == 1)
		pthread_cancel(m_thread_id);
}

int Thread::start()
{
	int result = pthread_create(&m_thread_id,NULL,runThread,this);
	if(result == 0)
		m_started = true;
	
	return result;
}

int Thread::join()
{
	if( !m_joined )
	{
		if( !pthread_join(m_thread_id,NULL) )
		{
			m_joined = true;
		}
		return 0;
	}
	return -1;
}

int Thread::detach()
{
	int result = -1;
	if( m_started == 1 && m_joined == 1)
	{
		result = pthread_detach(m_thread_id);
		if( result == 0 )
			m_joined = 0;
	}
	return result;
}
