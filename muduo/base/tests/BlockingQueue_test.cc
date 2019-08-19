#include <muduo/base/BlockingQueue.h>
#include <muduo/base/CountDownLatch.h>
#include <muduo/base/Thread.h>

#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <string>
#include <stdio.h>

class Test
{
 public:
  Test(int numThreads)
    : latch_(numThreads),
      threads_(numThreads)
  {
     for (int i = 0; i < numThreads; ++i) {
      char name[32];
      snprintf(name, sizeof name, "work thread %d", i);
      threads_.push_back(new muduo::Thread(
            boost::bind(&Test::threadFunc, this), name));
    }
    for_each(threads_.begin(), threads_.end(), boost::bind(&muduo::Thread::start, _1));
  }

  void run(int times) {
    printf("I am %s, waiting for count down latch\n", muduo::CurrentThread::name());
    latch_.wait();		//主线程等待所有子线程初始化完毕
    printf("all threads started, %s start create task\n", muduo::CurrentThread::name());
	
    for (int i = 0; i < times; ++i) {
      char buf[32];
      snprintf(buf, sizeof buf, "hello %d", i);
      queue_.put(buf);
      printf("tid=%d, put data=%s, queue.size=%zd\n", muduo::CurrentThread::tid(), buf, queue_.size());
    }
  }

  void joinAll() {
    for (size_t i = 0; i < threads_.size(); ++i) {
      queue_.put("stop");
    }

    for_each(threads_.begin(), threads_.end(), boost::bind(&muduo::Thread::join, _1));
  }


 private:
  void threadFunc() {
    printf("tid=%d, %s started\n",
           muduo::CurrentThread::tid(),
           muduo::CurrentThread::name());

    latch_.countDown();
    bool running = true;
	
    while (running) {
      std::string d(queue_.take());
      printf("I am %s, tid=%d, get data = %s, queue.size = %zd\n",
      		muduo::CurrentThread::name(), muduo::CurrentThread::tid(), d.c_str(), queue_.size());
      running = (d != "stop");
    }
    printf("tid=%d, %s stopped\n",
           muduo::CurrentThread::tid(),
           muduo::CurrentThread::name());
  }

  muduo::BlockingQueue<std::string> queue_;
  muduo::CountDownLatch latch_;
  boost::ptr_vector<muduo::Thread> threads_;
};


int main()
{
  printf("pid=%d, tid=%d\n", ::getpid(), muduo::CurrentThread::tid());
  
  Test t(4);
  t.run(20);	//主线程开始压入任务
  t.joinAll();	//结束线程

  printf("number of created threads %d\n", muduo::Thread::numCreated());
}
