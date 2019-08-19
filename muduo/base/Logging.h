#ifndef MUDUO_BASE_LOGGING_H
#define MUDUO_BASE_LOGGING_H

#include <muduo/base/LogStream.h>
#include <muduo/base/Timestamp.h>

namespace muduo
{

class Logger
{
 public:
  /*日志级别*/
  enum LogLevel {
    TRACE,			//0 指出比DEBUG粒度更细的一些信息事件(开发过程中使用)
    DEBUG,			//1 指出细粒度信息事件(开发过程中使用）
    INFO,			//2 表明消息在粗粒度级别上突出强调应用程序的运行过程（muduo默认这个级别）
    WARN,			//3 系统能正常运行，但可能会出现潜在错误的情形
    ERROR,			//4 指出虽然发生错误事件，但仍然不影响系统的继续运行
    FATAL,			//5 指出每个严重的错误事件，将会导致应用程序的退出
    NUM_LOG_LEVELS  //6 日志级别数
  };

  // compile time calculation of basename of source file
  class SourceFile
  {
   public:
    template<int N>
    inline SourceFile(const char (&arr)[N]) //注意这里不能加explicit，否则后面Logger的构造函数无法隐式构造SourceFile
      : data_(arr),
        size_(N-1)
    {
      const char* slash = strrchr(data_, '/'); // builtin function
      if (slash)
      {
        data_ = slash + 1;
        size_ -= static_cast<int>(data_ - arr);
      }
    }

    explicit SourceFile(const char* filename)
      : data_(filename)
    {
      const char* slash = strrchr(filename, '/');  //找到filename中最后一个/字符
      if (slash)
      {
        data_ = slash + 1;
      }
      size_ = static_cast<int>(strlen(data_));
    }

    const char* data_;
    int size_;
  };

public:
  Logger(SourceFile file, int line);  //对应INFO级别
  Logger(SourceFile file, int line, LogLevel level);  //对应WARN、ERROR、FATAL级别
  Logger(SourceFile file, int line, LogLevel level, const char* func);  //对应TRACE、DEBUG级别
  Logger(SourceFile file, int line, bool toAbort);  //对应SYSERR、SYSFATAL级别
  ~Logger();

  //这里返回Logger的输出流对象Logger.impl_.stream_
  LogStream& stream() { return impl_.stream_; }

  static LogLevel logLevel();
  static void setLogLevel(LogLevel level);

  typedef void (*OutputFunc)(const char* msg, int len);
  typedef void (*FlushFunc)();
  static void setOutput(OutputFunc);
  static void setFlush(FlushFunc);
  
 private:
  class Impl	 //其构造函数实现Logger功能
  {
   public:
    typedef Logger::LogLevel LogLevel;
    Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
    void formatTime();
    void finish();

    Timestamp time_;	 //输出日志的当前时间
    LogStream stream_;	 //日志输出流对象
    LogLevel level_;	 //日志级别
    int line_;			 //输出日志的行
    SourceFile basename_;//输出日志的文件名
  };


private:  
  Impl impl_;
};


extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel()
{
  return g_logLevel;
}

//以下是用户输出日志的接口，先构造一个Logger对象，在其中向日志输出流中输出时间、线程等系统信息，
//然后调用其stream函数返回日志输出流对象，用户通过日志输出流的<<运算符向流中写入用户信息。
//最后由于这里构造的是匿名对象，构造完以后会被析构，在Logger的析构函数中将流冲刷到标准输出设备。

//这里__FILE__会隐式转换调用SourceFile的构造函数构造出一个SourceFile对象

//以下是应用级别的错误
#define LOG_TRACE if (muduo::Logger::logLevel() <= muduo::Logger::TRACE) \
  muduo::Logger(__FILE__, __LINE__, muduo::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (muduo::Logger::logLevel() <= muduo::Logger::DEBUG) \
  muduo::Logger(__FILE__, __LINE__, muduo::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (muduo::Logger::logLevel() <= muduo::Logger::INFO) \
  muduo::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN muduo::Logger(__FILE__, __LINE__, muduo::Logger::WARN).stream()
#define LOG_ERROR muduo::Logger(__FILE__, __LINE__, muduo::Logger::ERROR).stream()
#define LOG_FATAL muduo::Logger(__FILE__, __LINE__, muduo::Logger::FATAL).stream()
//以下是系统级别的错误，日志会根据errno值输出对应的系统错误
#define LOG_SYSERR muduo::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL muduo::Logger(__FILE__, __LINE__, true).stream() //传入true表示要终止程序

const char* strerror_tl(int savedErrno);

// Taken from glog/logging.h
//
// Check that the input is non NULL.  This very useful in constructor
// initializer lists.

#define CHECK_NOTNULL(val) \
  ::muduo::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

// A small helper for CHECK_NOTNULL().
template <typename T>
T* CheckNotNull(Logger::SourceFile file, int line, const char *names, T* ptr) {
  if (ptr == NULL) {
   Logger(file, line, Logger::FATAL).stream() << names;
  }
  return ptr;
}

}

#endif  // MUDUO_BASE_LOGGING_H
