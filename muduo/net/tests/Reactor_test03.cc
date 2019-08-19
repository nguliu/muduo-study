#include <muduo/net/Channel.h>
#include <muduo/net/EventLoop.h>

#include <boost/bind.hpp>

#include <stdio.h>
#include <sys/timerfd.h>

using namespace muduo;
using namespace muduo::net;

EventLoop* g_loop;
int timerfd;

void timeout(Timestamp receiveTime)
{
	printf("Timeout!\n");
	uint64_t howmany;
	
	/*将可读事件读走，如果不读走将会一直触发可读事件*/
	::read(timerfd, &howmany, sizeof howmany);

	/*退出loop循环*/
	g_loop->quit();
}

int main(void)
{
	EventLoop loop;
	g_loop = &loop;

//timerfd_create把时间变成一个文件描述符，该“文件”在超时的时候变得可读，
//这样就可使用poll/epoll模型来统一处理IO事件和超时事件
//CLOCK_MONOTONIC可理解为定时时间段
//CLOCK_REALTIME可理解为定时到时间点，手动更改系统时间到定时点之后会触发定时器
	timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	Channel channel(&loop, timerfd);
	channel.setReadCallback(boost::bind(timeout, _1));
	channel.enableReading();

	struct itimerspec howlong;
	bzero(&howlong, sizeof howlong);
	howlong.it_value.tv_sec = 1;	//it_value = 1s,it_interval = 0
									//即定时1秒的一次性定时器
	::timerfd_settime(timerfd, 0, &howlong, NULL);	//设定到定时器

	loop.loop();

	::close(timerfd);
}



