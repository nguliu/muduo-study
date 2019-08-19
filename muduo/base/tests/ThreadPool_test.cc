#include <muduo/base/ThreadPool.h>
#include <muduo/base/CountDownLatch.h>
#include <muduo/base/CurrentThread.h>

#include <boost/bind.hpp>
#include <stdio.h>

void print() {
  printf("I am %s, tid = %d, get a empty task\n", muduo::CurrentThread::name(), muduo::CurrentThread::tid());
}

void printString(const std::string& text) {
  printf("I am %s, tid = %d, text = %s\n",
  	muduo::CurrentThread::name(), muduo::CurrentThread::tid(), text.c_str());
}

void countDown(muduo::ThreadPool* pool, muduo::CountDownLatch* latch) {
	printf("there are task countDown, I am %s, will sleep 2s.......\n", muduo::CurrentThread::name());
	sleep(2);
	pool->run(boost::bind(&muduo::CountDownLatch::countDown, latch));
}

int main()
{
  printf("main: pid = %d, tid = %d\n", ::getpid(), muduo::CurrentThread::tid());

  muduo::ThreadPool pool("MainThreadPool");
  pool.start(5);	//创建5个线程并启动

  pool.run(print);
  pool.run(print);
  
  for (int i = 0; i < 20; ++i) {
    char buf[32];
    snprintf(buf, sizeof buf, "task %d come from thread %d", i, muduo::CurrentThread::tid());
    pool.run(boost::bind(printString, std::string(buf)));
  }

  muduo::CountDownLatch latch(1);	//传入一个countDown任务
  pool.run(boost::bind(&countDown, &pool, &latch));
  
  latch.wait();
  pool.stop();
}