// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include <muduo/base/Exception.h>
//#include <muduo/base/CurrentThread.h>
#include <cxxabi.h>
#include <execinfo.h>
#include <stdlib.h>
#include <stdio.h>


namespace muduo
{

//这里为了实现阶段化学习对原著有改动，原版对后面的CurrentThread.{h,cc}等文件有依赖

Exception::Exception(string msg)
  : message_(std::move(msg))
//	stack_(CurrentThread::stackTrace(false))
{
	/* 填充调用栈信息 */
	fillStackTrance();
}

/* 获取调用栈信息并将其写到 stack_ */
void Exception::fillStackTrance() {
	const int len = 200;	//调用栈深度
	void* buffer[len];

	/* backtrace进行栈回溯，保存各个栈帧的地址到buffer中，返回栈帧的深度 */
	int nptrs = ::backtrace(buffer, len);

	/* 将调用栈帧的地址转换为函数名，返回保存函数名的二维数组 */
	char** strings = ::backtrace_symbols(buffer, nptrs);
	
	if (strings) {
		for (int i = 0; i < nptrs; ++i) {
			//TODO demangle funcion name with::__cxa_demangle
			//stack_.append(strings[i]);		  //no demangle
			stack_.append(demangle(strings[i]));  //demangled
			
			stack_.push_back('\n');
		}
		
		free(strings);  // strings指向的内存是malloc出来的，需要自己释放
	}
}

string Exception::demangle(const char* symbol)
{
  size_t size;
  int status;
  char temp[128];
  char* demangled;
  //first, try to demangle a c++ name
  if (1 == sscanf(symbol, "%*[^(]%*[^_]%127[^)+]", temp)) {
	if (NULL != (demangled = abi::__cxa_demangle(temp, NULL, &size, &status))) {
	  string result(demangled);
	  free(demangled);
	  return result;
	}
  }
  //if that didn't work, try to get a regular c symbol
  if (1 == sscanf(symbol, "%127s", temp)) {
	return temp;
  }
 
  //if all else fails, just return the symbol
  return symbol;
}


}  // namespace muduo
