#include <muduo/base/Logging.h>
#include <muduo/base/LogFile.h>
#include <muduo/base/ThreadPool.h>

#include <stdio.h>

int g_total = 0;
FILE* g_file = nullptr;
boost::scoped_ptr<muduo::LogFile> g_logFile;

void dummyOutput(const char* msg, int len)
{
  g_total += len;
  if (g_file)
  {
    fwrite(msg, 1, len, g_file);
  }
  else if (g_logFile)
  {
    g_logFile->append(msg, len);
  }
}

void bench(const char* type)
{
  muduo::Timestamp start(muduo::Timestamp::now());
  
  g_total = 0;
  int n = 1000*1000;
  const bool kLongLog = false;
  muduo::string empty = " ";
  muduo::string longStr(3000, 'X');
  longStr += " ";
  
  for (int i = 0; i < n; ++i) {
    LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz"
             << (kLongLog ? longStr : empty)
             << i;
  }
  muduo::Timestamp end(muduo::Timestamp::now());
  double seconds = timeDifference(end, start);
  printf("%20s: %f seconds, %d bytes, %10.2f msg/s, %.2f MiB/s\n",
         type, seconds, g_total, n / seconds, g_total / seconds / (1024 * 1024));
}

void logInThread()
{
  LOG_INFO << "logInThread";
  usleep(1000);
}

int main()
{
  getppid(); // for ltrace and strace
  muduo::Logger::setLogLevel(muduo::Logger::TRACE);

  muduo::ThreadPool pool("main-thread-pool");
  pool.start(5);
  pool.run(logInThread);
  pool.run(logInThread);
  pool.run(logInThread);
  pool.run(logInThread);
  pool.run(logInThread);

  LOG_TRACE << "trace";
  LOG_DEBUG << "debug";
  LOG_INFO  << "Hello";
  LOG_WARN  << "World";
  LOG_ERROR << "Error";
  LOG_INFO  << sizeof(muduo::Logger);
  LOG_INFO  << sizeof(muduo::LogStream);
  LOG_INFO  << sizeof(muduo::Fmt);
  LOG_INFO  << sizeof(muduo::LogStream::Buffer);

  sleep(1);
  muduo::Logger::setOutput(dummyOutput);
  bench("nop"); //空调用

  char buffer[64*1024];

  g_file = fopen("/dev/null", "w");
  setbuffer(g_file, buffer, sizeof buffer);
  bench("/dev/null");
  fclose(g_file);

  g_file = fopen("LogFile_bench.log", "w");
  setbuffer(g_file, buffer, sizeof buffer);
  bench("LogFile_bench.log");
  fclose(g_file);

  g_file = NULL;
  g_logFile.reset(new muduo::LogFile("test_log_t_nosafe", 500*1000*1000, false));
  bench("test_log_t_nosafe");

  g_logFile.reset(new muduo::LogFile("test_log_t_safe", 500*1000*1000, true));
  bench("test_log_t_safe");
  g_logFile.reset();
}
