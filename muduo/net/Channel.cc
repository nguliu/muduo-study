// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include <muduo/base/Logging.h>
#include <muduo/net/Channel.h>
#include <muduo/net/EventLoop.h>

#include <sstream>

#include <poll.h>

using namespace muduo;
using namespace muduo::net;

/*poll事件:

  POLLIN		  有数据可读。
  
　POLLPRI		  有紧迫数据可读。

　POLLOUT		  写数据不会导致阻塞。

　POLLHUP　　 		  指定的文件描述符挂起事件。

  POLLRDHUP		  对等方关闭
  
　POLLRDNORM		  有普通数据可读。

　POLLRDBAND		  有优先数据可读。

　POLLWRNORM		  写普通数据不会导致阻塞。

　POLLWRBAND	      写优先数据不会导致阻塞。

　POLLMSGSIGPOLL   消息可用。

　POLLERR　　   	  指定的文件描述符发生错误。

　POLLNVAL　　		  指定的文件描述符非法。
*/

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd__)
  : loop_(loop),
    fd_(fd__),
    events_(0),
    revents_(0),
    index_(-1),
    logHup_(true),
    tied_(false),
    eventHandling_(false)
{
}

Channel::~Channel()
{
  assert(!eventHandling_);
}

//这里进行生存期管理，用tie_保存obj的弱指针
void Channel::tie(const boost::shared_ptr<void>& obj)
{
  tie_ = obj;
  tied_ = true;
}

void Channel::update()
{
  loop_->updateChannel(this);
  //调用所属EventLoop的updateChannel，再调用poller的updateChannel更新关注的事件
}

//从当前channel所属的Eventloop中将当前channel移除
void Channel::remove()
{
  assert(isNoneEvent()); //确保已经没有关注当前channel的任何事件
  
  loop_->removeChannel(this);
}


//事件到来时调用handleEvent进行处理
void Channel::handleEvent(Timestamp receiveTime)
{
  boost::shared_ptr<void> guard;
  if (tied_)			  //如果进行了生存期管理，先尝试将弱指针提升为强指针，
  {						  //若提升成功才进行事件处理，否则说明当前对象已经不存在
    guard = tie_.lock();  //尝试提升弱指针
    if (guard)
    {
      handleEventWithGuard(receiveTime);
    }
  }
  else
  {
    handleEventWithGuard(receiveTime);
  }
}


//以下根据当前channel的事件调用对应的处理函数
void Channel::handleEventWithGuard(Timestamp receiveTime)
{
  eventHandling_ = true;
  
  if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {  //对等方挂起(HUP)事件
    if (logHup_) {
      LOG_WARN << "Channel::handle_event() POLLHUP";
    }
    if (closeCallback_)
		closeCallback_();
  }
  if (revents_ & POLLNVAL) {	//指定非法文件描述符
    LOG_WARN << "Channel::handle_event() POLLNVAL";
  }

  if (revents_ & (POLLERR | POLLNVAL)) {  //套接字非法或错误
    if (errorCallback_)
		errorCallback_();
  }
  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
    if (readCallback_)
		readCallback_(receiveTime);
  }
  if (revents_ & POLLOUT) {
    if (writeCallback_)
		writeCallback_();
  }
  
  eventHandling_ = false;
}

string Channel::eventsToString() const
{
  std::ostringstream oss;
  if (events_ == 0) {
	oss << "null ";
	return oss.str().c_str();
  }
   
  if (events_ & POLLIN)
    oss << "IN ";
  if (events_ & POLLPRI)
    oss << "PRI ";
  if (events_ & POLLOUT)
    oss << "OUT ";
  if (events_ & POLLHUP)
    oss << "HUP ";
  if (events_ & POLLRDHUP)
    oss << "RDHUP ";
  if (events_ & POLLERR)
    oss << "ERR ";
  if (events_ & POLLNVAL)
    oss << "NVAL ";

  return oss.str().c_str();
}

string Channel::reventsToString() const
{
  std::ostringstream oss;
  oss << "fd=" << fd_ << " revents: ";
  if (revents_ & POLLIN)
    oss << "IN ";
  if (revents_ & POLLPRI)
    oss << "PRI ";
  if (revents_ & POLLOUT)
    oss << "OUT ";
  if (revents_ & POLLHUP)
    oss << "HUP ";
  if (revents_ & POLLRDHUP)
    oss << "RDHUP ";
  if (revents_ & POLLERR)
    oss << "ERR ";
  if (revents_ & POLLNVAL)
    oss << "NVAL ";

  return oss.str().c_str();
}
