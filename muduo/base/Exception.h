// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_EXCEPTION_H
#define MUDUO_BASE_EXCEPTION_H

#include <muduo/base/Types.h>
#include <execinfo.h>
#include <exception>

namespace muduo
{

class Exception : public std::exception
{
 public:
  Exception(string what);
  ~Exception() noexcept override = default;

  // default copy-ctor and operator= are okay.

  //返回异常信息
  const char* what() const noexcept override
  {
    return message_.c_str();
  }

  //返回调用栈信息
  const char* stackTrace() const noexcept
  {
    return stack_.c_str();
  }

 private:
  void fillStackTrance();			//获取当前函数调用栈
  string demangle(const char* symbol);		//此函数用于demangle，将编译器修改后的函数名还原
  string message_;		//保存异常信息
  string stack_;		//保存函数调用栈
};

}  // namespace muduo

#endif  // MUDUO_BASE_EXCEPTION_H
