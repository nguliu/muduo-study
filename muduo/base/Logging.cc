#include <muduo/base/Logging.h>

#include <muduo/base/CurrentThread.h>
#include <muduo/base/StringPiece.h>
#include <muduo/base/Timestamp.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sstream>

namespace muduo
{

/*
class LoggerImpl
{
 public:
  typedef Logger::LogLevel LogLevel;
  LoggerImpl(LogLevel level, int old_errno, const char* file, int line);
  void finish();

  Timestamp time_;
  LogStream stream_;
  LogLevel level_;
  int line_;
  const char* fullname_;
  const char* basename_;
};
*/

__thread char t_errnobuf[512];
__thread char t_time[32];     //保存当前线程格式化的时间（年.月.日-时.分.秒）
__thread time_t t_lastSecond;	//上一次格式化时间的秒

//这里有一个技巧：在一秒内写入两条日志时，秒及以上的时间都不用再次格式化，因为在一秒之内
//它们肯定都没有边，在前端线程日志量巨大是对性能有极大地提升。（亲自体会过，血淋淋的教训）

const char* strerror_tl(int savedErrno)	//根据错误代码返回错误描述
{
  return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
}


//初始化系统日志级别
Logger::LogLevel initLogLevel() {
  if (::getenv("MUDUO_LOG_TRACE"))
    return Logger::TRACE;
  else if (::getenv("MUDUO_LOG_DEBUG"))
    return Logger::DEBUG;
  else
    return Logger::INFO;
}

Logger::LogLevel g_logLevel = initLogLevel();

const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
  "TRACE ",
  "DEBUG ",
  " INFO ",
  " WARN ",
  "ERROR ",
  "FATAL ",
};

// helper class for known string length at compile time
class T {
 public:
  T(const char* str, unsigned len)
    :str_(str),
     len_(len)
  {
    assert(strlen(str) == len_);
  }

  const char* str_;
  const unsigned len_;
};

//以下追加数据到LogStream的缓冲区
inline LogStream& operator<<(LogStream& s, T v) {
  s.append(v.str_, v.len_);
  return s;
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v) {
  s.append(v.data_, v.size_);
  return s;
}

//默认输出到标准输出
void defaultOutput(const char* msg, int len){
  size_t n = fwrite(msg, 1, len, stdout);
  (void)n; //FIXME check n
}
void defaultFlush() {
  fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;

}

using namespace muduo;


Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile& file, int line)
  : time_(Timestamp::now()),
    stream_(),
    level_(level),
    line_(line),
    basename_(file)
{
  formatTime();	 //格式化时间到缓冲区
  CurrentThread::tid();
  stream_ << T(CurrentThread::tidString(), 6);  //这里因为在cacheTid中将tid格式化为"%5d "格式
  stream_ << T(LogLevelName[level], 6);
  if (savedErrno != 0)
  {
    stream_ << strerror_tl(savedErrno) << "(errno=" << savedErrno << ") ";
  }

  //到这里系统信息已全部写入流中，接下来由用户调用的<<运算符将用户数据写入流中
}

//格式化并输出时间
void Logger::Impl::formatTime() {
  int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / 1000000);
  int microseconds = static_cast<int>(microSecondsSinceEpoch % 1000000);
  if (seconds != t_lastSecond)
  {
    t_lastSecond = seconds;
    struct tm tm_time;
    ::gmtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime

    int len = snprintf(t_time, sizeof(t_time), "%4d.%02d.%02d-%02d:%02d:%02d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    assert(len == 19); (void)len;
  }
  Fmt us(".%06dZ ", microseconds); //这里将微秒格式化到us里
  assert(us.length() == 9);

  //格式化完成后输出时间到stream_的缓冲区
  stream_ << T(t_time, 19) << T(us.data(), 9);
}


//输出文件名和行号
void Logger::Impl::finish() {
  stream_ << " - " << basename_ << ':' << line_ << '\n';
}

//构造impl_时将全部系统信息格式化到流中
Logger::Logger(SourceFile file, int line)
  : impl_(INFO, 0, file, line)
{
}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func)
  : impl_(level, 0, file, line)
{
  impl_.stream_ << "in func[" << func << "] ";
}

Logger::Logger(SourceFile file, int line, LogLevel level)
  : impl_(level, 0, file, line)
{
}
  
//这里传入系统错误代码errno
Logger::Logger(SourceFile file, int line, bool toAbort)
  : impl_(toAbort ? FATAL : ERROR, errno, file, line)
{ //这里根据是否需要abort决定日志级别是FATAL还是ERROR
}

//在这里冲刷输出流
Logger::~Logger() {
  impl_.finish();
  const LogStream::Buffer& buf(stream().buffer());
  
  g_output(buf.data(), buf.length()); //将应用缓冲区的数据输出到系统输出缓冲区
  g_flush();
  
  if (impl_.level_ == FATAL)
  {
//  g_flush();
    abort();
  }
}

void Logger::setLogLevel(Logger::LogLevel level) {
  g_logLevel = level;
}

//通过以下接口可将日志写到其他输出设备中
void Logger::setOutput(OutputFunc out) {
  g_output = out;
}

void Logger::setFlush(FlushFunc flush) {
  g_flush = flush;
} 
