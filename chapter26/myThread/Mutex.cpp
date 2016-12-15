#include "Mutex.h"

#include <assert.h>

Mutex::Mutex(bool isprocess_shared):
	process_shared(isprocess_shared)
{
	pthread_mutexattr_t arrr_t;
	pthread_mutexattr_settype(&arrr_t,PTHREAD_MUTEX_NORMAL);
	if( process_shared )
	{
		int shared;  
        		pthread_mutexattr_getpshared(&arrr_t, &shared);  
    		assert(shared ==  PTHREAD_PROCESS_PRIVATE);  
        		pthread_mutexattr_setpshared(&arrr_t, PTHREAD_PROCESS_SHARED);
	}
	pthread_mutex_init(&m_mutex,&arrr_t);
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&m_mutex);
}

int Mutex::lock()
{
	return pthread_mutex_lock(&m_mutex);
}

int Mutex::unlock()
{
	return pthread_mutex_unlock(&m_mutex);
}
