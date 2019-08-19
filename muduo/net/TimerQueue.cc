// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#define __STDC_LIMIT_MACROS
#include <muduo/net/TimerQueue.h>

#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/Timer.h>
#include <muduo/net/TimerId.h>

#include <boost/bind.hpp>

#include <sys/timerfd.h>

namespace muduo
{
namespace net
{
namespace detail
{

// 创建定时器文件描述符
int createTimerfd()
{
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (timerfd < 0)
  {
    LOG_SYSFATAL << "Failed in timerfd_create";
  }
  return timerfd;
}

// 计算when与当前时间的时间差
struct timespec howMuchTimeFromNow(Timestamp when)
{
  int64_t microseconds = when.microSecondsSinceEpoch()
                         - Timestamp::now().microSecondsSinceEpoch();
  if (microseconds < 100)	//最小定时100微秒
  {
    microseconds = 100;
  }
  
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(
      microseconds / Timestamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(
      (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
  
  return ts;
}

//读取定时器文件描述符，避免一直触发
void readTimerfd(int timerfd, Timestamp now)
{
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
  LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toFormattedString(true);

  if (n != sizeof howmany)
  {
    LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
  }
}


// 注册定时器文件描述符超时时刻，这里注册的是一次性定时器
void resetTimerfd(int timerfd, Timestamp expiration)
{
  //wake up loop by timerfd_settime() 
//  struct itimerspec oldValue;
//  bzero(&oldValue, sizeof oldValue);
  struct itimerspec newValue;
  bzero(&newValue, sizeof newValue);
  
  newValue.it_value = howMuchTimeFromNow(expiration);  //这里设置的是一次性定时器
//  int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
  int ret = ::timerfd_settime(timerfd, 0, &newValue, nullptr);
  
  if (ret)
  {
    LOG_SYSERR << "timerfd_settime()";
  }
}

}
}
}

using namespace muduo;
using namespace muduo::net;
using namespace muduo::net::detail;


TimerQueue::TimerQueue(EventLoop* loop)
  : loop_(loop),
    timerfd_(createTimerfd()),
    timerfdChannel_(loop, timerfd_), //初始化定时器的通道
    timers_(),
    callingExpiredTimers_(false)
{
  //设置定时器通道的可读事件回调函数为handleRead
  timerfdChannel_.setReadCallback(
      boost::bind(&TimerQueue::handleRead, this));
  //这里无形参的handleRead可以被设置为有形参的ReadEventCallback类型函数
  //测试见extra_tests/Bind_without_reference_func_to_have_reference_func.cc
  
  // we are always reading the timerfd, we disarm it with timerfd_settime.
  timerfdChannel_.enableReading();	//关注定时器通道的可读事件
}

TimerQueue::~TimerQueue()
{
  ::close(timerfd_);
  
  // do not remove channel, since we're in EventLoop::dtor();
  for (TimerList::iterator it = timers_.begin();
      it != timers_.end(); ++it)
  {
    delete it->second;	//销毁所有定时器对象
  }
}

//对外接口：添加定时器（实际使用EventLoop中的接口）
TimerId TimerQueue::addTimer(const TimerCallback& cb,
                             Timestamp when,
                             double interval)
{
  Timer* timer = new Timer(cb, when, interval);	//注意要手动释放timer
  
  //在loop线程中异步的添加，在无锁状态下实现线程安全
  loop_->runInLoop(
      boost::bind(&TimerQueue::addTimerInLoop, this, timer));
  
  /*addTimerInLoop(timer);*/
  return TimerId(timer, timer->sequence());
}

//对外接口：取消定时器（实际使用EventLoop中的接口）
void TimerQueue::cancel(TimerId timerId)
{
  //在loop线程中异步的取消，在无锁状态下实现线程安全
  loop_->runInLoop(
      boost::bind(&TimerQueue::cancelInLoop, this, timerId));
  
  /*cancelInLoop(timerId);*/
}


//注意：在添加定时器时如果最早到期时间改变了需要从新设定定时器描述符的到期时间，这样可避免
//		定时器的过晚到期。但删除定时器时没有重新设定，即使被删除的定时器到期触发了可读事件，
//		通过getExpired获取的超时定时器列表为空，不会有任何影响。

//添加一个定时器
void TimerQueue::addTimerInLoop(Timer* timer)
{
  loop_->assertInLoopThread();
  
  // 插入一个定时器，有可能会使得最早到期的定时器发生改变
  bool earliestChanged = insert(timer);

  if (earliestChanged)
  {
    // 重置定时器的超时时刻(timerfd_settime)
    resetTimerfd(timerfd_, timer->expiration());
  }
}

//取消一个定时器
void TimerQueue::cancelInLoop(TimerId timerId)
{
  loop_->assertInLoopThread();
  assert(timers_.size() == activeTimers_.size());

  //这里根据TimerId快速找到定时器
  ActiveTimer timer(timerId.timer_, timerId.sequence_);
  ActiveTimerSet::iterator it = activeTimers_.find(timer);
  
  if (it != activeTimers_.end())  //如果在activeTimers_列表中，直接删除就可以了
  {
    size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
    assert(n == 1); (void)n;
    delete it->first; // FIXME: no delete please,如果用了unique_ptr,这里就不需要手动删除了
    activeTimers_.erase(it);
  }
  else if (callingExpiredTimers_) //如果不在activeTimers_列表中，可能是因为已经到期被移动到了expired中
  {								  //此时需要加入取消列表，避免被重新加入到定时器列表
    cancelingTimers_.insert(timer);
  }
  assert(timers_.size() == activeTimers_.size());
}


//处理到期的定时器列表
void TimerQueue::handleRead()
{
  loop_->assertInLoopThread();
  
  Timestamp now(Timestamp::now());
  readTimerfd(timerfd_, now);		// 清除该事件，避免一直触发

//  cancelingTimers_.clear();	//清空被取消的定时器列表
  
  // 获取当前时刻超时的所有定时器
  // 在getExpired之前调用cancelInLoop会将定时器直接删除，在getExpired之后
  // 调用cancelInLoop会将定时器加入cancelingTimers_列表
  std::vector<Entry> expired = getExpired(now);

  callingExpiredTimers_ = true;
  cancelingTimers_.clear();	//清空被取消的定时器列表
  
  // safe to callback outside critical section
  for (std::vector<Entry>::iterator it = expired.begin();
      it != expired.end(); ++it)
  {
    it->second->run(); //这里回调每个定时器实际的事件处理函数
  }
  callingExpiredTimers_ = false;

  // 对于到期的非一次性定时器，需要重新加入定时器列表
  reset(expired, now);
}


//获取当前时刻超时的所有定时器(rov优化)
std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
  assert(timers_.size() == activeTimers_.size());
  
  std::vector<Entry> expired;
  Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
  
  // 返回第一个未超时的Timer的迭代器
  // lower_bound的含义是返回第一个值>=sentry的元素的iterator
  // 即*end >= sentry，从而end->first > now
  TimerList::iterator end = timers_.lower_bound(sentry);
  assert(end == timers_.end() || now < end->first);
  
  std::copy(timers_.begin(), end, back_inserter(expired));

  // 从timers_中移除到期的定时器
  timers_.erase(timers_.begin(), end);
  
  // 从activeTimers_中移除到期的定时器
  for (std::vector<Entry>::iterator it = expired.begin();
      it != expired.end(); ++it)
  {
    ActiveTimer timer(it->second, it->second->sequence());
    size_t n = activeTimers_.erase(timer);
    assert(n == 1); (void)n;
  }

  assert(timers_.size() == activeTimers_.size());
  return expired;
}

//将expired中的非一次性定时器重新注册
void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
  Timestamp nextExpire;

  for (std::vector<Entry>::const_iterator it = expired.begin();
      it != expired.end(); ++it)
  {
    ActiveTimer timer(it->second, it->second->sequence());
	
    // 如果是重复的定时器并且是未取消定时器，则重启该定时器
    if (it->second->repeat()
        && cancelingTimers_.find(timer) == cancelingTimers_.end())
    {
      it->second->restart(now);
      insert(it->second);
    }
    else
    {
      // 一次性定时器或者已被取消的定时器是不能重置的，因此删除该定时器
      // FIXME move to a free list
      delete it->second; // FIXME: no delete please
    }
  }

  if (!timers_.empty())
  {
    // 获取最早到期的定时器超时时间
    nextExpire = timers_.begin()->second->expiration();
  }

  if (nextExpire.valid())
  {
    // 重置定时器的超时时刻(timerfd_settime)
    resetTimerfd(timerfd_, nextExpire);
  }
}


//加入一个新定时器
bool TimerQueue::insert(Timer* timer)
{
  loop_->assertInLoopThread();
  assert(timers_.size() == activeTimers_.size());
  
  // 最早到期时间是否改变
  bool earliestChanged = false;
  Timestamp when = timer->expiration();
  TimerList::iterator it = timers_.begin();  //找到原来最早到期的定时器
  // 如果timers_为空或者when小于timers_中的最早到期时间则最早到期时间发生改变
  if (it == timers_.end() || when < it->first)
  {
    earliestChanged = true;
  }
  {
    // 插入到timers_中
    std::pair<TimerList::iterator, bool> result
      = timers_.insert(Entry(when, timer));
    assert(result.second); (void)result;
  }
  {
    // 插入到activeTimers_中
    std::pair<ActiveTimerSet::iterator, bool> result
      = activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
    assert(result.second); (void)result;
  }

  assert(timers_.size() == activeTimers_.size());
  return earliestChanged;
}

