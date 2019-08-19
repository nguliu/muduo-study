// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_THREADLOCAL_H
#define MUDUO_BASE_THREADLOCAL_H

#include <boost/noncopyable.hpp>
#include <pthread.h>

//线程全局的私有数据
//通过__thread关键字也可以得到线程私有数据（如CurrentThread::t_threadName、CurrentThread::t_cachedTid），
//但此关键字只能修饰POD类型而无法修饰class，因此为了线程能持有任意类型的私有数据使用了以下的机制：

namespace muduo
{

//ThreadLocal类采用RAII机制管理线程私有数据的键
template<typename T>
class ThreadLocal : boost::noncopyable
{
 public:
  ThreadLocal() {
    pthread_key_create(&pkey_, &ThreadLocal::destructor);
	/*这里创建了一个与线程私有数据地址相关联的键，并为该键关联了析构函数。这个键
	 *是每个线程公有的，但每个线程可通过这个键关联一个线程私有数据，当线程调用
	 *pthread_exit或执行完毕等正常退出时，析构函数会被调用去析构当前线程的私有数据.
	 *如果线程以exit、_exit、_Exit、abort等非正常方式退出时不会调用析构函数
	 *详见APUE P328
	 */
  }

  ~ThreadLocal() {
    pthread_key_delete(pkey_);
	/*这里释放键，但这并不会导致与该键相关联的析构函数被调用
	 */
  }

  T& value() {
    T* perThreadValue = static_cast<T*>(pthread_getspecific(pkey_));
	//这里获取当前线程与该键相关联的私有数据的指针，如果当前线程未关联
	//线程私有数据，则返回nullptr
	
    if (!perThreadValue) {
      T* newObj = new T();
      pthread_setspecific(pkey_, newObj);	//将当前线程的私有数据与键相关联
      perThreadValue = newObj;
    }
    return *perThreadValue;
  }


 private:
  static void destructor(void *x) {	//销毁与键相关联的数据
    T* obj = static_cast<T*>(x);
    typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
	//由于销毁时只能是完全类型，这里检查T是否为完全类型，如果T是
	//不完全类型，则会定义一个大小为-1的数组，这样在编译时会报错
	T_must_be_complete_type str; (void)str;  //这一行用于消除定义但未使用警告
    delete obj;
  }

 private:
  pthread_key_t pkey_;	//用于关联线程私有数据地址的键
};

}
#endif
