#include <muduo/base/Condition.h>
#include <muduo/base/Thread.h>

#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <stdio.h>

using namespace muduo;


MutexLock mutex;
Condition condition(mutex);

void threadFunc(int i) {
	MutexLockGuard lockGuard(mutex);

	condition.wait();
	printf("Thread %d execute step one\n", i);

	condition.wait();
	printf("Thread %d execute step two\n", i);

	//这里一定要有unlock操作
//	mutex.unlock();
}


int main() {
	printf("main thread: pid = %d, tid = %d, start......\n", ::getpid(), CurrentThread::tid());
	
	const int tCount = 5;

	boost::ptr_vector<Thread> threads;
	for (int i = 0; i < tCount; ++i) {
		char name[32];
		snprintf(name, sizeof name, "thread %d", i);
		threads.push_back(new Thread(boost::bind(&threadFunc, i), name));
	}
	for_each(threads.begin(), threads.end(), boost::bind(&Thread::start, _1));

	//每 1s 一条输出
	for (int i = 0; i < tCount; ++i) {
		sleep(1);
		condition.notify();
	}
	
	sleep(1);
	condition.notifyAll();

	for_each(threads.begin(), threads.end(), boost::bind(&Thread::join, _1));

	printf("main thread: pid = %d, tid = %d, end......\n", ::getpid(), CurrentThread::tid());
	return 0;
}