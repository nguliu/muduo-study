#include <muduo/base/Thread.h>
#include <muduo/base/CurrentThread.h>

#include <string>
#include <boost/bind.hpp>
#include <stdio.h>

void threadFunc()
{
  printf("tid=%d\n", muduo::CurrentThread::tid());
}

void threadFunc2(int x, const muduo::Thread* t)
{
  printf("tid=%d, x=%d, t.name = %s\n", muduo::CurrentThread::tid(), x, t->name().c_str());
}

class Foo
{
 public:
  explicit Foo(double x) : x_(x) { }

  void memberFunc() {
    printf("tid=%d, Foo::x_=%f\n", muduo::CurrentThread::tid(), x_);
  }

  void memberFunc2(const std::string& text) {
    printf("tid=%d, Foo::x_=%f, text=%s\n", muduo::CurrentThread::tid(), x_, text.c_str());
  }

 private:
  double x_;
};


int main()
{
  printf("测试处1：CurrentThread::t_cachedTid = %d, CurrentThread::t_threadName = %s\n\n",
		  muduo::CurrentThread::t_cachedTid, muduo::CurrentThread::t_threadName);
  /*测试处1说明：这里存取的是主线程的数据，原因详见Thread.cc中init对象的说明*/

  printf("pid=%d, tid=%d\n", ::getpid(), muduo::CurrentThread::tid());

  muduo::Thread t1(threadFunc);	//使用默认的线程名
  t1.start();
  t1.join();

  muduo::Thread t2(boost::bind(threadFunc2, 22, &t2), "thread 2 name");
  t2.start();
  t2.join();

  Foo foo(87.53);
  //注意绑定成员函数时要把this指针传进去
  muduo::Thread t3(boost::bind(&Foo::memberFunc, &foo), "thread 3 name");
  t3.start();
  t3.join();

  muduo::Thread t4(boost::bind(&Foo::memberFunc2, boost::ref(foo), std::string("Shuo Chen")), "thread 4 name");
  t4.start();
  t4.join();

  printf("number of created threads %d\n", muduo::Thread::numCreated());
}
