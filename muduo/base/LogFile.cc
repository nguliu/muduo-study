#include <muduo/base/LogFile.h>
#include <muduo/base/Logging.h> // strerror_tl
#include <muduo/base/ProcessInfo.h>

#include <assert.h>
#include <stdio.h>
#include <time.h>

using namespace muduo;

//File使用RAII机制对日志文件封装，他操作具体的日志文件
//File类不保证线程安全，线程安全有上层的LogFile保证
class LogFile::File : boost::noncopyable
{
 public:
  explicit File(const string& filename)
    : fp_(::fopen(filename.data(), "ae")),
      writtenBytes_(0)
  {
    assert(fp_);
	//设定文件指针的缓冲区，如果不设定是默认的缓冲区，缓冲区满系统自动fflush到文件中
    ::setbuffer(fp_, buffer_, sizeof buffer_);
    // posix_fadvise POSIX_FADV_DONTNEED ?
  }

  ~File() {
    ::fclose(fp_);
  }

  //追加到文件缓冲区
  void append(const char* logline, const size_t len) {
    size_t n = write(logline, len); //已写入
    size_t remain = len - n;		//剩余
	
    while (remain > 0) {	//如果未写完需要循环写入
      size_t x = write(logline + n, remain);
      if (x == 0) {
        int err = ferror(fp_);
        if (err) {
          fprintf(stderr, "LogFile::File::append() failed %s\n", strerror_tl(err));
        }
        break;
      }
      n += x;
      remain -= x;
    }

    writtenBytes_ += len;
  }

  void flush() {
    ::fflush(fp_);
  }

  size_t writtenBytes() const { return writtenBytes_; }


 private:
  size_t write(const char* logline, size_t len) {
 #undef fwrite_unlocked
    return ::fwrite_unlocked(logline, 1, len, fp_);
 	//注意：这里没有采用线程安全的fwrite，而是采用了效率更高的非线程安全的写入方式，因为线程
 	//		安全性已由LogFile的threadSafe参数保证，这里没必要在使用线程安全的方法
  }

  FILE* fp_;	//文件指针
  char buffer_[64*1024];	//被设定为fp_的系统缓冲区
  size_t writtenBytes_;		//已写入的字节数
};



LogFile::LogFile(const string& basename,
                 size_t rollSize,	//日志文件大小
                 bool threadSafe,   //默认设置为线程安全
                 int flushInterval) //冲刷时间间隔
  : basename_(basename),
    rollSize_(rollSize),
    flushInterval_(flushInterval),
    mutex_(threadSafe ? new MutexLock : NULL),	//通过mutex可保证线程安全
    startOfPeriod_(0),
    lastRoll_(0),
    lastFlush_(0),
    count_(0)
{
  assert(basename.find('/') == string::npos);
  rollFile();	//这里滚动一下日志，产生第一个日志文件
}

LogFile::~LogFile()
{
}

// 选择性的保证线程安全
void LogFile::append(const char* logline, int len) {
  if (mutex_) {
    MutexLockGuard lock(*mutex_);
    append_unlocked(logline, len);
  }
  else {
    append_unlocked(logline, len);
  }
}

void LogFile::flush() {
  if (mutex_) {
    MutexLockGuard lock(*mutex_);
    file_->flush();
  }
  else {
    file_->flush();
  }
}

//private:
void LogFile::append_unlocked(const char* logline, int len) {
  file_->append(logline, len);

  if (file_->writtenBytes() > rollSize_) {	 //文件大小驱动滚动
    rollFile();
  }
  else
  {
    if (count_ > kCheckTimeRoll_)  //写入次数超过kCheckTimeRoll_次才检测是否有时间驱动或是否冲刷
    {
      count_ = 0;
      time_t now = ::time(NULL);
      time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;	//将写入的时间对齐到当天零点
	  
      if (thisPeriod_ != startOfPeriod_) {	//时间驱动滚动
        rollFile();
      }
      else if (now - lastFlush_ > flushInterval_) {  //如果冲刷时间间隔超过3s则进行冲刷
        lastFlush_ = now;
        file_->flush();
      }
    }
    else
    {
      ++count_;
    }
  }
}

void LogFile::rollFile()
{
  time_t now = 0;
  string filename = getLogFileName(basename_, &now);

  //这里进行了对齐操作，对齐到当天零点
  time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

  if (now > lastRoll_)  //如果是新的一天
  {
    lastRoll_ = now;
    lastFlush_ = now;
    startOfPeriod_ = start;
	
    file_.reset(new File(filename));
	//这里使file_指向一个新的File对象(构造时fopen(filename)), 上一个File对象被销毁且保存日志文件
  }
}

string LogFile::getLogFileName(const string& basename, time_t* now)
{
  string filename;
  filename.reserve(basename.size() + 64);
  filename = basename;

  char timebuf[32];
  char pidbuf[32];
  
  struct tm tm;
  *now = time(NULL);   // UTC时间戳
  //FIXME: localtime_r
//gmtime_r(now, &tm);  // 将UTC时间戳转换为标准格式, gmtime_r相对于gmtime是线程安全的
  localtime_r(now, &tm);
  strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);

  filename += timebuf;
  filename += ProcessInfo::hostname();
  snprintf(pidbuf, sizeof pidbuf, ".p%d", ProcessInfo::pid());
  filename += pidbuf;
  filename += ".log";

  return filename;
}

