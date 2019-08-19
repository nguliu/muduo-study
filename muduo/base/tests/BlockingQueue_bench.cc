#include <muduo/base/BlockingQueue.h>
#include <muduo/base/CountDownLatch.h>
#include <muduo/base/Thread.h>
#include <muduo/base/Timestamp.h>

#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <map>
#include <string>
#include <stdio.h>

class Bench
{
 public:
  Bench(int numThreads)
    : latch_(numThreads),
      threads_(numThreads)
  {
    for (int i = 0; i < numThreads; ++i) {
      char name[32];
      snprintf(name, sizeof name, "work thread %d", i);
      threads_.push_back(new muduo::Thread(
            boost::bind(&Bench::threadFunc, this), name));
    }
    for_each(threads_.begin(), threads_.end(), boost::bind(&muduo::Thread::start, _1));
  }

  void run(int times)
  {
    printf("I am %s, waiting for count down latch\n", muduo::CurrentThread::name());
    latch_.wait();		//主线程等待所有子线程初始化完毕
    printf("all threads started, %s start create task\n", muduo::CurrentThread::name());
	
    for (int i = 0; i < times; ++i) {
      muduo::Timestamp now(muduo::Timestamp::now());
      queue_.put(now);
      usleep(100);
    }
  }

  void joinAll() {
    for (size_t i = 0; i < threads_.size(); ++i) {
      queue_.put(muduo::Timestamp::invalid());
    }
    for_each(threads_.begin(), threads_.end(), boost::bind(&muduo::Thread::join, _1));
  }
  

 private:
  void threadFunc() {
    printf("tid=%d, %s started\n",
           muduo::CurrentThread::tid(),
           muduo::CurrentThread::name());

    std::map<int, int> delays;
    latch_.countDown();
    bool running = true;
	
    while (running)
    {
      muduo::Timestamp t(queue_.take());
      muduo::Timestamp now(muduo::Timestamp::now());
	  	
      if (t.valid()) {
        int delay = static_cast<int>(timeDifference(now, t) * 1000000);
        // printf("tid=%d, latency = %d us\n",
        //        muduo::CurrentThread::tid(), delay);
        ++delays[delay];
      }
      running = t.valid();
    }

    for (std::map<int, int>::iterator it = delays.begin();
        it != delays.end(); ++it)
    {
      printf("tid = %d, delay = %d us, count = %d\n",
             muduo::CurrentThread::tid(),
             it->first, it->second);
    }
		
	printf("tid=%d, %s stopped\n",
           muduo::CurrentThread::tid(),
           muduo::CurrentThread::name());
  }

  muduo::BlockingQueue<muduo::Timestamp> queue_;
  muduo::CountDownLatch latch_;
  boost::ptr_vector<muduo::Thread> threads_;
};


int main(int argc, char* argv[])
{
  int threads = argc > 1 ? atoi(argv[1]) : 4;

  Bench t(threads);
  t.run(100);
  
  t.joinAll();
}
