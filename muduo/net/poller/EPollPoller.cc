// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include <muduo/net/poller/EPollPoller.h>

#include <muduo/base/Logging.h>
#include <muduo/net/Channel.h>

#include <boost/static_assert.hpp>

#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>

using namespace muduo;
using namespace muduo::net;

// On Linux, the constants of poll(2) and epoll(4)
// are expected to be the same.
BOOST_STATIC_ASSERT(EPOLLIN == POLLIN);
BOOST_STATIC_ASSERT(EPOLLPRI == POLLPRI);
BOOST_STATIC_ASSERT(EPOLLOUT == POLLOUT);
BOOST_STATIC_ASSERT(EPOLLRDHUP == POLLRDHUP);
BOOST_STATIC_ASSERT(EPOLLERR == POLLERR);
BOOST_STATIC_ASSERT(EPOLLHUP == POLLHUP);


//以下表示fd在epoll中的状态
namespace {
const int kNew = -1;	//空白状态：epoll未关注过该fd，channels_中也没有该fd到对应channel的存储（对应poll#87:channel->index() < 0）
const int kAdded = 1;   //关注状态：epoll正在关注该fd，channels_中有该fd到对应channel的存储（对应poll#121:pfd.fd > 0）
const int kDeleted = 2; //保留状态：epoll停止了对该fd的关注，channels_中保留该fd到对应channel的存储（对应poll#115:pfd.fd < 0）
}

EPollPoller::EPollPoller(EventLoop* loop)
  : Poller(loop),
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)),  //epoll_create1相比epoll_create可以设置句柄属性
    events_(kInitEventListSize)
{
  if (epollfd_ < 0)
  {
    LOG_SYSFATAL << "EPollPoller::EPollPoller:epoll_create1 error";
  }
}

EPollPoller::~EPollPoller()
{
  ::close(epollfd_);
}


Timestamp EPollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
  int numEvents = ::epoll_wait(epollfd_,
                               &*events_.begin(),
                               static_cast<int>(events_.size()),
                               timeoutMs);
  Timestamp now(Timestamp::now());
  if (numEvents > 0)
  {
    LOG_TRACE << numEvents << " events happended in EPollPoller";
	
    fillActiveChannels(numEvents, activeChannels); //将活动的通道填充到activeChannels
	
// 优化位置2：避免内存拷贝带来性能损失------------------------------------------------------------------------------------------
// 这里原本直接resize可能会有元素的拷贝，会有效率损失，我以swap方式扩大进行优化
// if (implicit_cast<size_t>(numEvents) == events_.size()) {
//    events_.resize(events_.size() * 2);
// }

// vector的swap操作只交换内部的start、finish等指针（测试见 extra_tests/Vector_swap_test.cc），
// 以这样的方式扩大events_不会有元素拷贝，效率更高，events_内的数据会丢失。
// 实际在一次poll查询中，到这里之后events_内的数据已无价值。
   if (implicit_cast<size_t>(numEvents) == events_.size()) {
		EventList tmp(events_.size() * 2);
		events_.swap(tmp);
   		assert(events_.size() == tmp.size() * 2);
	}   //离开作用域后events_原来的内存(被tmp接管)自动释放
//----------------------------------------------------------------------------------------------------------------------
  }
  else if (numEvents == 0)
  {
    LOG_INFO << " nothing happended in EPollPoller";
  }
  else
  {
    LOG_SYSERR << "EPollPoller::poll()";
  }
  return now;
}

/*epoll_event结构如下：
	struct epoll_event {
		uint32_t	 events;	// Epoll return events
		epoll_data_t data;		// User data variable
	};
	typedef union epoll_data {
       void    *ptr;
       int      fd;
       uint32_t u32;
       uint64_t u64;
   } epoll_data_t;
*/
void EPollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
  assert(implicit_cast<size_t>(numEvents) <= events_.size());
  
  for (int i = 0; i < numEvents; ++i)
  {
    Channel* channel = static_cast<Channel*>(events_[i].data.ptr); //这里在epoll_event中保留了channel的指针
	
#ifndef NDEBUG
    int fd = channel->fd();
    ChannelMap::const_iterator it = channels_.find(fd);
    assert(it != channels_.end());
    assert(it->second == channel);
#endif

    channel->setRevents(events_[i].events);  //将socket的事件设置到对应的channel
    activeChannels->push_back(channel);	 //将对应的channel加入活跃channel列表
  }
}

void EPollPoller::updateChannel(Channel* channel)
{
  Poller::assertInLoopThread();
  const int index = channel->index();
  
  LOG_TRACE << "EPollPoller.updateChannel: channel.fd=" << channel->fd() << " channel.events="
  			<< channel->eventsToString() << " channel.state="
  			<< (index < 0 ? "New" : (index == 1 ? "Concerned" : "Retained"));

  if (index == kNew || index == kDeleted)
  {
    // a unconcerned one, add with EPOLL_CTL_ADD
    int fd = channel->fd();
	
    if (index == kNew) {
      assert(channels_.find(fd) == channels_.end());
      channels_[fd] = channel;
    }
    else // index == kDeleted
    {
      assert(channels_.find(fd) != channels_.end());
      assert(channels_[fd] == channel);
    }
	
    channel->setIndex(kAdded);
    update(EPOLL_CTL_ADD, channel);	//添加该channel的事件关注
  }
  else  //kAdded: update existing one with EPOLL_CTL_MOD/DEL
  {
    int fd = channel->fd(); (void)fd;
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(index == kAdded);
	
    if (channel->isNoneEvent()) //如果不关注当前channel的任何事件了，将channel设为保留状态
    {
      update(EPOLL_CTL_DEL, channel); //删除该channel的事件关注
      channel->setIndex(kDeleted);
    }
    else	//否则只是修改epoll的关注事件，channel仍为关注状态
    {
      update(EPOLL_CTL_MOD, channel); //修改该channel的事件关注
    }
  }
}

void EPollPoller::removeChannel(Channel* channel)
{
  Poller::assertInLoopThread();
  int fd = channel->fd();
  int index = channel->index();

  LOG_TRACE << "EPollPoller.removeChannel: channel.fd=" << channel->fd() << " channel.events="
  			<< channel->eventsToString() << " channel.state="
  			<< (index < 0 ? "New" : (index == 1 ? "Concerned" : "Retained"));
  
  assert(channels_.find(fd) != channels_.end());
  assert(channels_[fd] == channel);
  assert(channel->isNoneEvent());	//一定是没有关注任何事件的channel才能移除
  assert(index == kAdded || index == kDeleted);

  size_t n = channels_.erase(fd);
  assert(n == 1); (void)n;

  if (index == kAdded) {
    update(EPOLL_CTL_DEL, channel);
  }
  channel->setIndex(kNew);
}

void EPollPoller::update(int operation, Channel* channel)
{
  struct epoll_event event;
  bzero(&event, sizeof event);
  
  event.events = channel->events();
  event.data.ptr = channel;	  //注意在这里保存当前channel的指针到epoll_event结构中
  int fd = channel->fd();
  
  if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
  {
    if (operation == EPOLL_CTL_DEL)
    {
      LOG_SYSERR << "epoll_ctl op=" << operation << " fd=" << fd;
    }
    else
    {
      LOG_SYSFATAL << "epoll_ctl op=" << operation << " fd=" << fd;
    }
  }
}

