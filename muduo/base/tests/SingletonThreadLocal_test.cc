#include <muduo/base/Singleton.h>
#include <muduo/base/CurrentThread.h>
#include <muduo/base/ThreadLocal.h>
#include <muduo/base/Thread.h>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <stdio.h>

class Test : boost::noncopyable
{
 public:
  Test() {
    printf("tid = %d, constructing %p\n", muduo::CurrentThread::tid(), this);
  }

  ~Test() {
    printf("tid = %d, destructing %p %s\n", muduo::CurrentThread::tid(), this, name_.c_str());
  }

  const std::string& name() const { return name_; }
  void setName(const std::string& n) { name_ = n; }

 private:
  std::string name_;
};


//产生一个单例的线程局部数据类对象ThreadLocal，他又产生一个局部对象Test
//注意：这里instance是Singleton的成员，他返回的是一个ThreadLocal对象，又调用
//ThreadLocal的value才得到当前线程的Test对象
#define STL muduo::Singleton<muduo::ThreadLocal<Test>>::instance().value()


void print() {
  printf("tid = %d, %p name = %s\n",
         muduo::CurrentThread::tid(),
         &STL,
         STL.name().c_str());
}

void threadFunc(const char* changeTo) {
  print();
  STL.setName(changeTo);
  sleep(1);
  print();
}

int main()
{
  STL.setName("main one");
  
  muduo::Thread t1(boost::bind(threadFunc, "thread1"));
  muduo::Thread t2(boost::bind(threadFunc, "thread2"));
  
  t1.start();
  t2.start();
  
  t1.join();
  print();

  t2.join();
  
  pthread_exit(0);
}
