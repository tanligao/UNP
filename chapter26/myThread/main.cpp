#include <iostream>
#include "Thread.h"

#define MAX_RUN 10

using namespace std;

void func1()
{
	for(int i = 0; i < MAX_RUN; ++i)
	{
		cout << "func1 pthread_t: " << pthread_self() << endl;
		sleep(1);	
	}
}

void func2()
{
	for(int i = 0; i < MAX_RUN; ++i)
	{
		cout << "func2 pthread_t: " << pthread_self() << endl;
		sleep(1);	
	}
}

int main(int argc,char *argv[])
{
	myThread thread1(func1);
	myThread thread2(func2);

	thread1.start();
	thread2.start();

	thread2.join();
	thread1.join();

	cout << "main thread done" << endl;
	return 0;
}
