// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_THREADLOCALSINGLETON_H
#define MUDUO_BASE_THREADLOCALSINGLETON_H

#include <boost/noncopyable.hpp>
#include <assert.h>
#include <pthread.h>

namespace muduo
{
//线程私有单例类实现：
//即这个对象是每个线程私有的，且这个对象在每个线程中都只有一个

template<typename T>
class ThreadLocalSingleton : boost::noncopyable
{
 public:
  static T& instance() {
    if (!t_value_) {		  //这里保证对象在当前线程单例
      t_value_ = new T();
      deleter_.set(t_value_); //将单例对象关联到键
    }
    return *t_value_;
  }

  static T* pointer() {
    return t_value_;
  }


 private:
  static void destructor(void* obj) {
    assert(obj == t_value_);
    typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1]; //保证T是完全类型
	T_must_be_complete_type str; (void)str;
    delete t_value_;
    t_value_ = 0;
  }

  //使用RAII机制封装pthread_key_t的创建与销毁
  class Deleter
  {
   public:
    Deleter() {
      pthread_key_create(&pkey_, &ThreadLocalSingleton::destructor);
	  //这里关联了键值的析构函数
    }
    ~Deleter() {
      pthread_key_delete(pkey_);
    }

    void set(T* newObj) {
	  assert(pthread_getspecific(pkey_) == NULL); //每个线程只关联一次	  
      pthread_setspecific(pkey_, newObj);
    }

    pthread_key_t pkey_;  //这个键值还是所有线程公有的
  };

private:
  static __thread T* t_value_;	//这里保证对象的线程私有。由于指针是POD类型，因此可以用__thread修饰
  static Deleter deleter_;
};


template<typename T>
__thread T* ThreadLocalSingleton<T>::t_value_ = nullptr;

template<typename T>
typename ThreadLocalSingleton<T>::Deleter ThreadLocalSingleton<T>::deleter_;

}
#endif
