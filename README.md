# muduo-study
这是对muduo网络库进行学习的总结，个人根据视屏课程进行学习（B站：https://www.bilibili.com/video/av45560719  网盘：https://pan.baidu.com/s/1m2hsx7BCCWN578tiZD_quA    hhyg）  
在这份源码中我对两个位置进行了细微的优化，对效率有略微的提升（具体修改位置见"课程学习记录.txt"）

# muduo主要技术点有：

1.固定大小的线程池封装，避免线程频繁创建销毁的开销  
2.方便的日志输出接口、使用双缓冲区实现异步日志、日志文件的滚动  
3.为了减少资源泄漏，使用了智能指针等RAII机制  
4.简易的HTTP服务器实现，使用状态机解析HTTP请求
