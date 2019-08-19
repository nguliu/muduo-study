// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include <muduo/base/Thread.h>
#include <muduo/base/CurrentThread.h>
#include <muduo/base/Exception.h>
//#include <muduo/base/Logging.h>

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

namespace muduo
{

namespace CurrentThread
{
  __thread int t_cachedTid = 0;	 // 线程真实pid（tid）的缓存，为了减少
								 // ::syscall(SYS_gettid)系统调用的次数提高获取tid的效率
  __thread char t_tidString[32];
  __thread const char* t_threadName = "unknown";
  
  const bool sameType = boost::is_same<int, pid_t>::value;
  BOOST_STATIC_ASSERT(sameType);
}
//以上__thread修饰的变量是每个线程私有的数据，只能够用于POD类型的数据
//对比Thread_test.cc中测试处1和以下测试处2可知，虽然测试处1主线程的以上数据已被初始化，
//但从测试处2看出所有其他子线程的以上数据都未被初始化。证明以上数据确实是每个线程私有。


namespace detail
{

pid_t gettid()
{
  return static_cast<pid_t>(::syscall(SYS_gettid));
  //函数::gettid可以得到线程的真实tid，但glibc没有实现该函数，
  //只能通过linux系统调用syscall(SYS_gettid)来获取
}


/*这里被新创建的子进程调用，新创建的子进程只有一个线程即主线程*/
void afterFork()
{
  muduo::CurrentThread::t_threadName = "main thread";
  muduo::CurrentThread::t_cachedTid = 0;  //这里子进程会拷贝父进程中调用fork的线程的t_cachedTid，
  										  //但这不是子进程中主线程的真实ID，所以这里置0便于下面进行缓存
  										  
  CurrentThread::tid();		//上面已将t_cachedTid清0，这里会缓存当前线程的真实ID
  // no need to call pthread_atfork(NULL, NULL, &afterFork);
}


/*这个类用于初始化程序第一个线程的以上私有数据*/
class ThreadNameInitializer
{
 public:
  ThreadNameInitializer()
  {
    muduo::CurrentThread::t_threadName = "main thread-main process";
    CurrentThread::tid();
    pthread_atfork(NULL, NULL, &afterFork);	//登记fork后子进程调用的函数
  }
};


ThreadNameInitializer init;
//这里定义一个全局的对象，该对象先于main函数进行构造，在构造函数中对主线程的t_cachedTid
//和t_threadName私有数据进行初始化，去除后通过Thread_test.cc中测试处1可看到效果。

}
}



using namespace muduo;

void CurrentThread::cacheTid()
{
  if (t_cachedTid == 0)
  {
    t_cachedTid = detail::gettid();
    int n = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
    assert(n == 6); (void) n;
  }
}

//通过线程真实ID和进程PID判断是否为主线程
bool CurrentThread::isMainThread()
{
  return tid() == ::getpid();
}


//-----------------------------------------------------------------------------------------------------

AtomicInt32 Thread::numCreated_;

Thread::Thread(const ThreadFunc& func, const string& n)
  : started_(false),
    pthreadId_(0),
    tid_(0),
    func_(func),
    name_(n)
{
  setDefaultName();
}

Thread::~Thread()
{
  // no join
}

void Thread::setDefaultName()
{
  int num = numCreated_.incrementAndGet();
  if (name_.empty())
  {
    char buf[32];
    snprintf(buf, sizeof buf, "Thread%d in process%d", num, ::getpid());
    name_ = buf;
  }
}

//创建一个线程
void Thread::start()
{
  assert(!started_);
  started_ = true;

  //注意只有在入口函数startThread之后的调用才是在子线程中执行，否则都是在主线程中执行。这里要把this指针传过去
  errno = pthread_create(&pthreadId_, NULL, &startThread, this);
  if (errno != 0)
  {
    //LOG_SYSFATAL << "Failed in pthread_create";
  }
}

int Thread::join()
{
  assert(started_);
  return pthread_join(pthreadId_, NULL);
}

void* Thread::startThread(void* obj)
{
  Thread* thread = static_cast<Thread*>(obj);
  thread->runInThread();
  return NULL;
}

void Thread::runInThread()
{
/*  printf("测试处2：CurrentThread::t_cachedTid = %d, CurrentThread::t_threadName = %s\n",
			muduo::CurrentThread::t_cachedTid, muduo::CurrentThread::t_threadName); */
  
  tid_ = CurrentThread::tid();	//这里才初始化了当前线程的私有数据
  muduo::CurrentThread::t_threadName = name_.c_str();
  
/*  printf("测试处3：CurrentThread::t_cachedTid = %d, CurrentThread::t_threadName = %s\n\n",
		  muduo::CurrentThread::t_cachedTid, muduo::CurrentThread::t_threadName); */
  try {
    func_();
    muduo::CurrentThread::t_threadName = "finished";
  }
  catch (const Exception& ex)
  {
    muduo::CurrentThread::t_threadName = "crashed";
    fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
    fprintf(stderr, "reason: %s\n", ex.what());
    fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
    abort();
  }
  catch (const std::exception& ex)
  {
    muduo::CurrentThread::t_threadName = "crashed";
    fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
    fprintf(stderr, "reason: %s\n", ex.what());
    abort();
  }
  catch (...)
  {
    muduo::CurrentThread::t_threadName = "crashed";
    fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
    throw; // rethrow
  }
}

