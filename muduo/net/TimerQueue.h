// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is an internal header file, you should not include this.

#ifndef MUDUO_NET_TIMERQUEUE_H
#define MUDUO_NET_TIMERQUEUE_H

#include <set>
#include <vector>

#include <boost/noncopyable.hpp>

#include <muduo/base/Mutex.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/Channel.h>

namespace muduo
{
namespace net
{

class EventLoop;
class Timer;
class TimerId;

///
/// A best efforts timer queue.
/// No guarantee that the callback will be on time.
///
class TimerQueue : boost::noncopyable
{
 public:
  TimerQueue(EventLoop* loop);
  ~TimerQueue();

  ///
  /// Schedules the callback to be run at given time,
  /// repeats if @c interval > 0.0.
  ///
  /// Must be thread safe. Usually be called from other threads.
  // 一定是线程安全的，可以跨线程调用。通常情况下被其它线程调用。
  TimerId addTimer(const TimerCallback& cb,
                   Timestamp when,
                   double interval);

  void cancel(TimerId timerId);


 private:
  // FIXME: use unique_ptr<Timer> instead of raw pointers.
  // unique_ptr是C++ 11标准的一个独享所有权的智能指针
  typedef std::pair<Timestamp, Timer*> Entry;	//存储定时器超时时刻和定时器地址
  typedef std::set<Entry> TimerList;
  typedef std::pair<Timer*, int64_t> ActiveTimer;	//存储定时器地址和序号，即TimerId的成员
  typedef std::set<ActiveTimer> ActiveTimerSet;

  // 以下成员函数只可能在其所属的I/O线程中调用，不存在竞争，不加锁可减少锁开销
  void addTimerInLoop(Timer* timer);
  void cancelInLoop(TimerId timerId);
  
  //timerFdChannel回调函数，注册到timerfdChannel_的readCallback中，当timerfd可读时
  //实际调用handleRead处理超时的定时器
  void handleRead();
  
  // move out all expired timers
  // 返回当前时刻超时的定时器列表
  std::vector<Entry> getExpired(Timestamp now);
  //重新设置expired内的非一次性定时器
  void reset(const std::vector<Entry>& expired, Timestamp now);

  bool insert(Timer* timer);	//插入一个定时器

  EventLoop* loop_;			// 所属EventLoop
  const int timerfd_;		// 定时器文件描述符，通过它注册和查询定时器事件
  Channel timerfdChannel_;	// 定时器文件描述符对应的通道
  
  TimerList timers_;		// 先按到期时间排序，到期时间相同的按Timer地址排序（set存储pair的排序测试见extra_tests/Set_save_pair.cc）
  ActiveTimerSet activeTimers_;	//按Timer地址排序，与timers_保存的是相同的内容
/*设计两个列表的原因：第一个按超时时间排序，方便找到某个时刻之前所有超时的定时器（但同一时刻可能有两个定时器）
					  第二个按定时器地址排序，即TimerId的成员属性，这样便于通过TimerId快速的找到某个定时器
 */
  bool callingExpiredTimers_;	   //是否在处理到期的定时器
  ActiveTimerSet cancelingTimers_; //保存的是被取消的定时器，按定时器地址排序
};

}
}
#endif  // MUDUO_NET_TIMERQUEUE_H
