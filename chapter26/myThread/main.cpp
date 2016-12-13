#include <iostream>
#include "Thread.h"
using namespace std;

int main(int argc,char *argv[])
{
	myThread *thread1 = new myThread();
	myThread *thread2 = new myThread();

	thread1->start();
	thread2->start();

	thread2->join();
	thread1->join();

	cout << "main thread done" << endl;
	return 0;
}
