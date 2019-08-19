// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include <muduo/net/Timer.h>

using namespace muduo;
using namespace muduo::net;

AtomicInt64 Timer::s_numCreated_;

void Timer::restart(Timestamp now)
{
  //如果是重复定时器，将当前时间加上时间间隔得到下一个超时时刻
  if (repeat_)
  {
    // 重新计算下一个超时时刻
    expiration_ = addTime(now, interval_);
  }
  //否则将下一个超时时刻设置为一个非法时刻
  else
  {
    expiration_ = Timestamp::invalid();
  }
}
