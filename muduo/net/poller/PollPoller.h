// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is an internal header file, you should not include this.

#ifndef MUDUO_NET_POLLER_POLLPOLLER_H
#define MUDUO_NET_POLLER_POLLPOLLER_H

#include <muduo/net/Poller.h>

#include <map>
#include <vector>

struct pollfd;

namespace muduo
{
namespace net
{

///
/// IO Multiplexing with poll(2).
///
/// 对poll的封装，在这里channel中的index_表示当前fd对应的pollfd结构在pollfds_中的下标
class PollPoller : public Poller
{
 public:

  PollPoller(EventLoop* loop);
  virtual ~PollPoller();

  virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels);
  virtual void updateChannel(Channel* channel);
  virtual void removeChannel(Channel* channel);


 private:
  typedef std::vector<struct pollfd> PollFdList;
  typedef std::map<int, Channel*> ChannelMap;   // key是文件描述符，value是Channel

  void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

  PollFdList pollfds_;	 //当前Poller所关注的fd的pollfd集合体
  ChannelMap channels_;	 //保存每个fd到对应channel的映射
};

}
}
#endif  // MUDUO_NET_POLLER_POLLPOLLER_H
