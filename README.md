# muduo-study
这是对muduo网络库进行学习的总结，个人根据视屏课程进行学习（B站：https://www.bilibili.com/video/av45560719  网盘：https://pan.baidu.com/s/1m2hsx7BCCWN578tiZD_quA    hhyg）  
在这份源码中我对两个位置进行了细微的优化，对效率有略微的提升（具体修改位置见"课程学习记录.txt"）

muduo是一个高质量的事件驱动型的网络库，使用的non-blocking IO(IO multiplexing)+ one loop per thread模型。此模型每个IO线程里面只有一个事件循环(即一个Reactor)，处理读写和定时事件，激活的事件通过回调方式提供用户处理业务逻辑。  
在linux下的话，可以把事件当做一个文件描述符，换句话也就是说一个file descriptor只能由一个线程读写。一个线程最多只有一个EventLoop，而EventLoop中的循环即是在不停的监视这些描述符，当描述符可读或可写的时候，通过回调函数提供给用户处理。  
这样我们可以很方便地把不同的socket套接字的描述符放到不同的线程去， 也可以把一些socket放到一个线程里，这样这些socket就是线程安全的，因为始终只有EventLoo所在线程在读写它们，极大的降低了我们的编程复杂性。

# Envoirment
- OS: Ubuntu 14.04
- Complier: g++ 7.4.0
# muduo主要技术点有：
1.使用了Poll、Epoll水平触发的IO多路复用技术，非阻塞IO并实现应用层缓冲区，使用Multiple Reactors模式  
2.IO模型为：Multiple Reactors(one loop per thread) + non-blocking IO + IO buffer  
3.主线程只负责accept请求，并以Round Robin的方式分发给其它IO线程(兼计算线程)，锁的争用只会出现在主线程和某一特定线程中  
4.将定时器注册到timer_fd，实现了定时器事件和IO事件通过Reactor模式统一进行处理，使用小根堆管理定时器  
5.方便的日志输出接口、使用双缓冲区实现异步日志、实现了日志文件的滚动  
6.实现了固定大小的线程池，避免线程频繁创建销毁的开销  
7.为了减少资源泄漏，使用了智能指针等RAII机制  
8.简易的HTTP服务器实现，使用状态机解析HTTP请求  
9.支持优雅关闭连接  
