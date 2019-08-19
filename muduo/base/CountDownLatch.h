// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_COUNTDOWNLATCH_H
#define MUDUO_BASE_COUNTDOWNLATCH_H

#include <muduo/base/Condition.h>
#include <muduo/base/Mutex.h>

#include <boost/noncopyable.hpp>

namespace muduo
{

// CountDownLatch即门栓类，当count_减为0时向所有线程发起通知
// 既可以用于所有子线程等待主线程发起起跑命令，
// 也可以用于主线程等待子线程初始完毕才开始工作
class CountDownLatch : boost::noncopyable
{
 public:
  explicit CountDownLatch(int count);

  void wait();

  void countDown();

  int getCount() const;

 private:
  mutable MutexLock mutex_;	//注意这里加了mutable，表示const成员函数(如getCount)可以修改他的状态
  Condition condition_;
  int count_;
};

}
#endif  // MUDUO_BASE_COUNTDOWNLATCH_H
