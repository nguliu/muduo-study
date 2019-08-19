#include <muduo/base/AsyncLogging.h>
#include <muduo/base/CurrentThread.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <muduo/base/Thread.h>

#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

off_t kRollSize = 500 * 1000 * 1000;

muduo::AsyncLogging* g_asyncLog = NULL;

void asyncOutput(const char* msg, int len)
{
  g_asyncLog->append(msg, len);
}

void bench()
{
  printf("pid = %d, tid = %d\n", getpid(), muduo::CurrentThread::tid());

  bool longLog = true;
  int cnt = 0;
  const int kBatch = 1000;
  muduo::string empty = " ";
  muduo::string longStr(3000, 'X');
  longStr += " ";

  for (int t = 0; t < 30; ++t)
  {
    muduo::Timestamp start = muduo::Timestamp::now();
    for (int i = 0; i < kBatch; ++i)
    {
      LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz "
               << (longLog ? longStr : empty)
               << cnt;
      ++cnt;
    }
    muduo::Timestamp end = muduo::Timestamp::now();
    printf("%f\n", timeDifference(end, start)*1000000/kBatch);
    struct timespec ts = { 0, 500*1000*1000 };
    nanosleep(&ts, NULL);
  }
}


int main(int argc, char* argv[])
{
  {
    // set max virtual memory to 2GB.
    size_t kOneGB = 1000*1024*1024;
    rlimit rl = { 2*kOneGB, 2*kOneGB };
    setrlimit(RLIMIT_AS, &rl);
  }

  char name[64] = { 0 };  
  strncpy(name, argv[0], sizeof name - 1);
  muduo::AsyncLogging asyncLog(::basename(name), kRollSize);
  g_asyncLog = &asyncLog;
  asyncLog.start();

  muduo::Logger::setOutput(asyncOutput);

  boost::ptr_vector<muduo::Thread> threads;
  for (int i = 0; i < 5; ++i) {
      snprintf(name, sizeof name, "work-thread-%d", i);
      threads.push_back(new muduo::Thread(&bench, name));
   }

   for_each(threads.begin(), threads.end(), boost::bind(&muduo::Thread::start, _1));
   for_each(threads.begin(), threads.end(), boost::bind(&muduo::Thread::join, _1));

   printf("All over!\n");
}
