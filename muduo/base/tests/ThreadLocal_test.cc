#include <muduo/base/ThreadLocal.h>
#include <muduo/base/CurrentThread.h>
#include <muduo/base/Thread.h>

#include <boost/noncopyable.hpp>
#include <stdio.h>

class Test : boost::noncopyable
{
 public:
  Test() {
    printf("tid = %d, constructing Test %p\n", muduo::CurrentThread::tid(), this);
  }

  ~Test() {
    printf("tid = %d, destructing Test %p %s\n", muduo::CurrentThread::tid(), this, name_.c_str());
  }

  const std::string& name() const { return name_; }
  void setName(const std::string& n) { name_ = n; }

 private:
  std::string name_;
};


muduo::ThreadLocal<Test> testObj1;	//这里会生成两个键
muduo::ThreadLocal<Test> testObj2;


void print() {
  printf("tid = %d,     obj1 value:%p name = %s\n",
         muduo::CurrentThread::tid(),
	 	 &testObj1.value(), testObj1.value().name().c_str());
  
  printf("tid = %d,     obj2 value:%p name = %s\n",
         muduo::CurrentThread::tid(),
	 	 &testObj2.value(), testObj2.value().name().c_str());
}

void threadFunc() {
  testObj1.value();	 //子线程通过两个键值关联两个对象
  testObj2.value();

  print();
  testObj1.value().setName("child thread changed 1");
  testObj2.value().setName("child thread changed 2");
  print();

//  exit(0);	//如果是这样退出将没有child thread changed 1/2的析构
}

int main() {
  testObj1.value();	 //主线程通过键值关联两个Test对象
  testObj2.value();

  testObj1.value().setName("main thread one");
  print();
  
  muduo::Thread t1(threadFunc);
  t1.start();
  t1.join();
  
  testObj2.value().setName("main thread two");
  print();

//  exit(0);	//如果是这样退出将没有main thread的析构
  pthread_exit(0);
 }
