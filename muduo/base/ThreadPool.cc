// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include <muduo/base/ThreadPool.h>

#include <muduo/base/Exception.h>

#include <boost/bind.hpp>
#include <assert.h>
#include <stdio.h>

using namespace muduo;

ThreadPool::ThreadPool(const string& name)
  : mutex_(),
    cond_(mutex_),
    name_(name),
    running_(false)
{
}

ThreadPool::~ThreadPool()
{
  if (running_) {
    stop();
  }
}

void ThreadPool::start(int numThreads)
{
  assert(threads_.empty());
  running_ = true;
  threads_.reserve(numThreads);
  
  for (int i = 0; i < numThreads; ++i) {
    char id[32];
    snprintf(id, sizeof id, " thread %d", i);
    threads_.push_back(new muduo::Thread(
          boost::bind(&ThreadPool::runInThread, this), name_+id));
    //threads_[i].start();
  }
  for_each(threads_.begin(), threads_.end(), boost::bind(&muduo::Thread::start, _1));
}

void ThreadPool::stop()
{
  {
    MutexLockGuard lock(mutex_);
    running_ = false;
    cond_.notifyAll();	//注意这里要唤醒所有线程，因为有的线程可能正在阻塞等待任务
    
    /*在唤醒线程之后、join线程之前一定要有unlock操作，因为在wait中等待的线程被唤醒后会
      进行lock，如果这里没有unlock将会造成其他线程死等，而此线程又在等待其他线程结束。
	 */
  }
  for_each(threads_.begin(), threads_.end(), boost::bind(&muduo::Thread::join, _1));
}

void ThreadPool::run(const Task& task)
{
  //如果线程池中没有线程，则由主线程直接执行
  if (threads_.empty()) {
    task();
  }
  //否则压入任务队列由线程池中的线程去执行
  else {
    MutexLockGuard lock(mutex_);
    queue_.push_back(task);
    cond_.notify();
  }
}

ThreadPool::Task ThreadPool::take()
{
  MutexLockGuard lock(mutex_);
  
  // always use a while-loop, due to spurious wakeup
  while (queue_.empty() && running_) {
    cond_.wait();
  }
  /*注意这里要在while循环中避免虚假唤醒*/
  
  Task task;
  if(!queue_.empty())
  {
    task = queue_.front();
    queue_.pop_front();
  }
  return task;
}

/*在这里真正的去取任务、执行任务*/
void ThreadPool::runInThread() {
  try {
    while (running_)
	{
      Task task(take());
	  
      if (task) {
        task();
      }
    }
  }
  catch (const Exception& ex) {
    fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
    fprintf(stderr, "reason: %s\n", ex.what());
    fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
    abort();
  }
  catch (const std::exception& ex) {
    fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
    fprintf(stderr, "reason: %s\n", ex.what());
    abort();
  }
  catch (...) {
    fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
    throw; // rethrow
  }
}

