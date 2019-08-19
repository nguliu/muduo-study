#ifndef MUDUO_BASE_LOGFILE_H
#define MUDUO_BASE_LOGFILE_H

#include <muduo/base/Mutex.h>
#include <muduo/base/Types.h>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace muduo
{

// 日志滚动实现，通过时间驱动(每天0点)或写满规定大小驱动，
// 用户选择性的设置是否需要保证线程安全，兼顾了效率和灵活性

class LogFile : boost::noncopyable
{
 public:
  LogFile(const string& basename,
          size_t rollSize,			//日志文件大小
          bool threadSafe = true,	//默认设置为线程安全的
          int flushInterval = 3);	//冲刷时间间隔默认为 3s
  ~LogFile();

  void append(const char* logline, int len);  //对外的接口，便于设置为Logger的g_output
  void flush();									//对外的接口，便于设置为Logger的g_flush


 private:
  void append_unlocked(const char* logline, int len);

  static string getLogFileName(const string& basename, time_t* now);
  void rollFile();				//滚动日志文件

  const string basename_;		//日志文件basename
  const size_t rollSize_;		//日志文件大小
  const int flushInterval_;		//日志写入文件的时间间隔(即冲刷缓冲区周期)

  boost::scoped_ptr<MutexLock> mutex_;
  time_t startOfPeriod_;	//开始记录日志的时间，自动对齐到当天零点。如果写日志的时间偏移到当天
  							//零点和startOfPeriod_相等说明还没到第二天，不会进行日志滚动
  time_t lastRoll_;			//上一次滚动日志的时间
  time_t lastFlush_;		//上一次冲刷日志缓冲区的时间

  int count_; //写日志计数器，如果等于kCheckTimeRoll_会检查是否需要冲刷或滚动日志
  
  class File;
  boost::scoped_ptr<File> file_; //指向当前日志文件

  const static int kCheckTimeRoll_ = 1024;
  const static int kRollPerSeconds_ = 24 * 60 * 60;  //驱动日志滚动的时间长度(24h * 60m * 60s)
};

}
#endif  // MUDUO_BASE_LOGFILE_H
