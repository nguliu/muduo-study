// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include <muduo/net/EventLoopThread.h>

#include <muduo/net/EventLoop.h>

#include <boost/bind.hpp>

using namespace muduo;
using namespace muduo::net;


EventLoopThread::EventLoopThread(const ThreadInitCallback& cb)
  : loop_(NULL),
    exiting_(false),
    thread_(boost::bind(&EventLoopThread::threadFunc, this)),
    mutex_(),
    cond_(mutex_),
    callback_(cb)
{
}

EventLoopThread::~EventLoopThread()
{
  exiting_ = true;
  loop_->quit();   //退出IO线程
  thread_.join();  //当前线程等待IO线程结束
}

EventLoop* EventLoopThread::startLoop()
{
  assert(!thread_.started());
  
  thread_.start();	//启动IO线程
  
  {
    MutexLockGuard lock(mutex_);
    while (loop_ == NULL) {   //当前线程等待IO线程初始化完成
      cond_.wait();
    }
  }

  return loop_;
}

//此函数是在新创建的IO线程中调用，和startLoop不是在同一个线程中调用
void EventLoopThread::threadFunc()
{
  EventLoop loop;

  if (callback_)
  {
    callback_(&loop);
  }

  {
    MutexLockGuard lock(mutex_);
    // loop_指针指向了一个栈上的对象，threadFunc函数退出之后，这个指针就失效了
    // threadFunc函数退出，就意味着线程退出了，EventLoopThread对象也就没有存在的价值了。
    // 因而不会有什么大的问题
    loop_ = &loop;
    cond_.notify();  //通知创建此线程的线程继续运行
  }

  loop.loop();
  //assert(exiting_);
}

