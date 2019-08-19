#include <muduo/base/CountDownLatch.h>
#include <muduo/base/Thread.h>

#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <string>
#include <stdio.h>

using namespace muduo;

class Test
{
 public:
  Test(int numThreads)		//注意这里latch值为1
    : latch_(1), threads_(numThreads)
  {
    for (int i = 0; i < numThreads; ++i)
    {
      char name[32];
      snprintf(name, sizeof name, "work thread %d", i);
      threads_.push_back(new muduo::Thread(
            boost::bind(&Test::threadFunc, this), name));
    }
    for_each(threads_.begin(), threads_.end(), boost::bind(&Thread::start, _1));
    //for (auto t : threads_)
	//	t->start();
  }

  void run()
  {
    latch_.countDown();
  }

  void joinAll()
  {
    for_each(threads_.begin(), threads_.end(), boost::bind(&Thread::join, _1));
  }


 private:
  void threadFunc()
  {
    latch_.wait();	 //等待门栓
    printf("tid = %d, %s started\n",
           CurrentThread::tid(),
           CurrentThread::name());

    printf("tid = %d, %s stopped\n",
           CurrentThread::tid(),
           CurrentThread::name());
  }

  CountDownLatch latch_;
  boost::ptr_vector<Thread> threads_;
};

int main()
{
//这里是主线程向其他线程发起跑命令

  Test t(4);

  printf("pid = %d, tid = %d, I am %s, I will sleep 5s......\n", ::getpid(), CurrentThread::tid(), CurrentThread::name());  
  sleep(5);

  printf("pid = %d, tid = %d %s running......\n", ::getpid(), CurrentThread::tid(), CurrentThread::name());
  t.run();		//发出起跑命令
  t.joinAll();

  printf("number of created threads %d\n", Thread::numCreated());
}


