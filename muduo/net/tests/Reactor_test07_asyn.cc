#include <muduo/net/Acceptor.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/SocketsOps.h>
#include <muduo/net/EventLoopThread.h>

#include <boost/bind.hpp>
#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

void newConnection(int sockfd, const InetAddress& peerAddr)
{
  printf("loop thread: pid = %d, tid = %d, accepted a new connection from %s\n",
         getpid(), CurrentThread::tid(), peerAddr.toIpPort().c_str());
  ::write(sockfd, "How are you?\n", 13);
  sockets::close(sockfd);
}



int main()
{
  printf("main(): pid = %d, tid = %d\n",
         getpid(), CurrentThread::tid());
  
  InetAddress listenAddr(8888);
  EventLoopThread loopThread;
  EventLoop* loop = loopThread.startLoop();
  
  Acceptor acceptor(loop, listenAddr);
  acceptor.setNewConnectionCallback(newConnection);
//  acceptor.listen();  //这样将会dump
  loop->runInLoop(boost::bind(&Acceptor::listen, &acceptor)); //这里异步调用listen，也只能在loop所属线程中调用

  while (true)
  {
	  sleep(10);
	  printf("main(): pid = %d, tid = %d\n",
         getpid(), CurrentThread::tid());
  }
  return 0;
}