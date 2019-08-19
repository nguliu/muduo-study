// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_THREAD_H
#define MUDUO_BASE_THREAD_H

#include <muduo/base/Atomic.h>
#include <muduo/base/Types.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <pthread.h>

namespace muduo
{

class Thread : boost::noncopyable
{
 public:
  typedef boost::function<void ()> ThreadFunc;

  explicit Thread(const ThreadFunc&, const string& n = string());
  ~Thread();

  void start();	//创建一个线程
  int join(); // return pthread_join()

  bool started() const { return started_; }
  // pthread_t pthreadId() const { return pthreadId_; }
  pid_t tid() const { return tid_; }
  const string& name() const { return name_; }
  //获取线程个数
  static int numCreated() { return numCreated_.get(); }


 private:
  void setDefaultName();
  static void* startThread(void* thread);	//线程入口函数（调用runInThread）
  //注意只有在入口函数startThread之后的调用才是在子线程中执行，否则都是在主线程中执行。
  void runInThread();		//线程执行函数（调用真正的任务函数func_）

  bool       started_;
  pthread_t  pthreadId_;	//进程内线程id
  pid_t      tid_;			//全局的线程真实id
  ThreadFunc func_;			//线程任务
  string     name_;			//线程名

  //线程个数，注意是静态的，并且是原子的
  static AtomicInt32 numCreated_;
};

}
#endif
