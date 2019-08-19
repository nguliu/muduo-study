// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_SINGLETON_H
#define MUDUO_BASE_SINGLETON_H

#include <boost/noncopyable.hpp>
#include <pthread.h>
#include <stdlib.h> // atexit

namespace muduo
{

//线程安全的单例类实现
template<typename T>
class Singleton : boost::noncopyable
{
 public:
  //返回线程安全的单例类对象
  static T& instance() {
    pthread_once(&ponce_, &Singleton::init);
    return *value_;
	//第一次调用instance时会调用init创建单例对象，pthread_once能保证init只被调用
  	//一次，并且是线程安全的（即有多个线程同时调用instance也保证init只被调用一次）
  }


 private:
  Singleton();
  ~Singleton();

  static void init() {
    value_ = new T();
    ::atexit(destroy);
	//登记一个销毁函数，在程序结束时自动调用
  }

  static void destroy() {
    typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
	T_must_be_complete_type str; (void)str;  //这一行用于消除定义但未使用警告
    delete value_;
	//由于销毁时只能是完全类型，这里检查T是否为完全类型，如果T是
	//不完全类型，则会定义一个大小为-1的数组，这样在编译时会报错
  }

 private:
  static pthread_once_t ponce_;	//这个变量能保证某个函数只被执行一次
  static T*             value_;
};


//初始化静态的成员变量
template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;

}
#endif

