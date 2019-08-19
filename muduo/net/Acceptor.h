// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is an internal header file, you should not include this.

#ifndef MUDUO_NET_ACCEPTOR_H
#define MUDUO_NET_ACCEPTOR_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <muduo/net/Channel.h>
#include <muduo/net/Socket.h>

namespace muduo
{
namespace net
{

class EventLoop;
class InetAddress;

///
/// Acceptor of incoming TCP connections.
///
class Acceptor : boost::noncopyable
{
 public:
   typedef boost::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;

  Acceptor(EventLoop* loop, const InetAddress& listenAddr);
  ~Acceptor();

  void setNewConnectionCallback(const NewConnectionCallback& cb)
  { newConnectionCallback_ = cb; }

  bool listenning() const { return listenning_; }
  void listen();


 private:
  void handleRead();		//它注册到channel的readCallback，用于接受新到来的连接

  EventLoop* loop_;			//所属的EventLoop对象
  Socket acceptSocket_;		//服务端监听套接字
  Channel acceptChannel_;   //监听套接字所属channel
  NewConnectionCallback newConnectionCallback_; //内部新连接回调函数，处理新建立的连接（设置为TcpServer的newConnection）
  bool listenning_;
  int idleFd_;		//用于优雅关闭连接保留的句柄
};

}
}

#endif  // MUDUO_NET_ACCEPTOR_H
