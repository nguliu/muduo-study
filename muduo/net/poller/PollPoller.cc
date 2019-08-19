// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include <muduo/net/poller/PollPoller.h>

#include <muduo/base/Logging.h>
#include <muduo/base/Types.h>
#include <muduo/net/Channel.h>

#include <assert.h>
#include <poll.h>

using namespace muduo;
using namespace muduo::net;

PollPoller::PollPoller(EventLoop* loop)
  : Poller(loop)
{
}

PollPoller::~PollPoller()
{
}


//查询有事件channel，将有事件的channel保存到activeChannels
Timestamp PollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
  // XXX pollfds_ shouldn't change
  int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
  Timestamp now(Timestamp::now());

  if (numEvents > 0) {
    LOG_TRACE << numEvents << " events happended in PollPoller";
	
    fillActiveChannels(numEvents, activeChannels);	//将活动通道填充到activeChannels
  }
  else if (numEvents == 0)
  {
    LOG_INFO << " nothing happended in PollPoller";
  }
  else
  {
    LOG_SYSERR << "PollPoller::poll()";
  }
  
  return now;
}


//将有事件的socket的revents填充到对应的channel，然后将有事件的channel填充到activeChannels
void PollPoller::fillActiveChannels(int numEvents,
                                    ChannelList* activeChannels) const
{
  for (PollFdList::const_iterator pfd = pollfds_.begin();
      pfd != pollfds_.end() && numEvents > 0; ++pfd)
  {
    if (pfd->revents > 0)	//这里是查找有事件的pollfd结构
    {
      --numEvents;
      ChannelMap::const_iterator ch = channels_.find(pfd->fd);
      assert(ch != channels_.end());
	  
      Channel* channel = ch->second;
      assert(channel->fd() == pfd->fd);
	  
      channel->setRevents(pfd->revents); //这里将socket的事件保存到他对应的channel中
      //pfd->revents = 0;
      activeChannels->push_back(channel);	//将有事件的channel加入到活动通道列表
    }
  }
}


void PollPoller::updateChannel(Channel* channel)
{
  Poller::assertInLoopThread();
  
  LOG_TRACE << "PollPoller.updateChannel: channel.fd=" << channel->fd() << " channel.events="
  			<< channel->eventsToString() << " channel.index=" << channel->index() ;

  if (channel->index() < 0) {  //说明这是一个新通道，将新通道加入到关注的集合
    assert(channels_.find(channel->fd()) == channels_.end());
	
    struct pollfd pfd;
    pfd.fd = channel->fd();
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    pollfds_.push_back(pfd);
	
    int idx = static_cast<int>(pollfds_.size())-1;
    channel->setIndex(idx);
    channels_[pfd.fd] = channel;
  }
  else  // update existing one
  {
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
	
    int idx = channel->index();
    assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));

    struct pollfd& pfd = pollfds_[idx];	//找到当前socket所对应的pollfd结构，注意一定要是引用
    assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd() - 1);

//	pfd.fd = channel->fd();  //当disableAll后再enableSomething时要将文件描述符设为合法
    pfd.events = static_cast<short>(channel->events());  //更新当前channel对应的pollfd的events
    pfd.revents = 0;
	
    if (channel->isNoneEvent())
    {
      // ignore this pollfd
	  // 这里pfd.fd直接设置为负数即可，这样设置是为了在removeChannel时能将其还原回去
      pfd.fd = -channel->fd()-1;   //-1是为了使0也能变成负数
    }
	else {
	  pfd.fd = channel->fd();  //当disableAll后再enableSomething时要将文件描述符设为合法
	}
  }
}


void PollPoller::removeChannel(Channel* channel)
{
  Poller::assertInLoopThread();
  
  LOG_TRACE << "PollPoller.removeChannel: channel.fd=" << channel->fd() << " channel.events="
  			<< channel->eventsToString() << " channel.index=" << channel->index();
  
  assert(channels_.find(channel->fd()) != channels_.end());
  assert(channels_[channel->fd()] == channel);
  assert(channel->isNoneEvent());  //确保channel中没有事件了
  
  int idx = channel->index();
  assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
  
  const struct pollfd& pfd = pollfds_[idx]; (void)pfd;
  assert(pfd.fd == -channel->fd() - 1);
  assert(pfd.events == channel->events()); //确保poller也没有关注fd的任何事件
  
  size_t n = channels_.erase(channel->fd());
  assert(n == 1); (void)n;


//注意这里有关键性操作：如果当前pollfd在pollfds_中是最后一个元素则直接移除，否则将待删除元素与
//最后一个元素交换之后再pop_back，因为如果直接将idx元素移除后，vector内部会将idx之后的所有元素
//向前拷贝一个位置，降低了效率。同时致命的是idx之后的所有元素的下标都要减1

  if (implicit_cast<size_t>(idx) == pollfds_.size()-1) {
    pollfds_.pop_back();
  }
  else {
    int channelAtEndFd = pollfds_.back().fd;
    iter_swap(pollfds_.begin()+idx, pollfds_.end()-1);
  
    if (channelAtEndFd < 0) //如果fd被修改为了非法的负数需要将其还原回去，这样才能找到channels_中对应的channel
	{
      channelAtEndFd = -channelAtEndFd - 1;
    }
    channels_[channelAtEndFd]->setIndex(idx); //注意一定要修改被交换的元素对应channel中保存的下标
    pollfds_.pop_back();
  }
}

