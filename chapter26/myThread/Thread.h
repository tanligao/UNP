#ifndef  __THREAD_H_
#define __THREAD_H_

#include <string>
#include <unistd.h>
#include <iostream>

using namespace std;

class noncopyable
{
protected:
	noncopyable(const noncopyable &);
	noncopyable& operator=(const noncopyable &);
public:
	noncopyable() {}
	~noncopyable() {}
};

class Thread : private noncopyable
{
public:
	// typedef void* (func*) (void *argc) thread_func;
	
	explicit Thread();
	~Thread();
	int start();
	virtual void* run()  = 0;
	int join();
	int detach();
	inline int gettid() { return m_thread_id; }
private:
	pthread_t m_thread_id;
	bool m_started;
	bool m_joined;
};

class myThread : public Thread
{
public:
	void* run()
	{
		for(int i = 0; i < 5; ++i)
		{
			cout << "thread " << gettid() << " runinig " << i + 1 << endl;
			sleep(2);
		}
		return NULL;
	}
};

#endif
