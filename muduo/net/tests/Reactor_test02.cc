#include <muduo/net/EventLoop.h>

#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

EventLoop* g_loop;

void threadFunc()
{
	g_loop->loop();		//这里跨线程调用EventLoop对象，会出错
}

int main(void)
{
	EventLoop loop;		//这是主线程的EventLoop对象
	g_loop = &loop;

	g_loop->loop();	
	
	Thread t(threadFunc);
	t.start();
	t.join();
	
	return 0;
}

