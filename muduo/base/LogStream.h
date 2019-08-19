#ifndef MUDUO_BASE_LOGSTREAM_H
#define MUDUO_BASE_LOGSTREAM_H

#include <muduo/base/StringPiece.h>
#include <muduo/base/Types.h>
#include <assert.h>
#include <string.h> // memcpy
#ifndef MUDUO_STD_STRING
#include <string>
#endif
#include <boost/noncopyable.hpp>

namespace muduo
{
namespace detail
{

const int kSmallBuffer = 4000;			// 3.9K
const int kLargeBuffer = 4000 * 1000;	// 3.8M

//缓冲区类，传入一个非类型参数指定缓冲区大小
template<int SIZE>
class FixedBuffer : boost::noncopyable  //应用层缓冲区
{
 public:
  FixedBuffer()
    : cur_(data_)
  {
    setCookie(cookieStart);
  }

  ~FixedBuffer() {
    setCookie(cookieEnd);
  }

  void append(const char* /*restrict*/ buf, size_t len) {
    // FIXME: append partially
    if (implicit_cast<size_t>(avail()) > len) {
      memcpy(cur_, buf, len);
      cur_ += len;
    }
  }

  const char* data() const { return data_; }
  int length() const { return static_cast<int>(cur_ - data_); }

  // write to data_ directly
  char* current() { return cur_; }
  int avail() const { return static_cast<int>(end() - cur_); }
  void add(size_t len) { cur_ += len; }

  void reset() { cur_ = data_; }
  void bzero() { ::bzero(data_, sizeof data_); }

  // for used by GDB
  const char* debugString(); //在末尾添加'\0'，将缓冲区数据作为字符串
  void setCookie(void (*cookie)()) { cookie_ = cookie; }
  // for used by unit test
  string asString() const { return string(data_, length()); }


 private:
  const char* end() const { return data_ + sizeof data_; }
  // Must be outline function for cookies.
  static void cookieStart();
  static void cookieEnd();

  void (*cookie_)();
  char data_[SIZE];	 //存储缓冲数据
  char* cur_;
};

}



class LogStream : boost::noncopyable
{
  typedef LogStream self;
 public:
  typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;


  self& operator<<(bool v) {
  //buffer_.append(v ? "1" : "0", 1);
	buffer_.append(v ? "true" : "false", v ? 4 : 5);
    return *this;
  }
  self& operator<<(const void*);
  self& operator<<(short);
  self& operator<<(unsigned short);
  self& operator<<(int);
  self& operator<<(unsigned int);
  self& operator<<(long);
  self& operator<<(unsigned long);
  self& operator<<(long long);
  self& operator<<(unsigned long long);

  self& operator<<(float v) {
    *this << static_cast<double>(v);  //将float转化为double进行缓存
    return *this;
  }
  self& operator<<(double);
//self& operator<<(long double);

  self& operator<<(char v) {
    buffer_.append(&v, 1);
    return *this;
  }
//self& operator<<(signed char);
//self& operator<<(unsigned char);
  self& operator<<(const char* v) {
    buffer_.append(v, strlen(v));
    return *this;
  }
  self& operator<<(const std::string& v) {
    buffer_.append(v.c_str(), v.size());
    return *this;
  }


//这里统一了以上char*和string的接口，并且不涉及内存拷贝，实现了高效的字符串传递
  self& operator<<(const StringPiece& v) {
    buffer_.append(v.data(), v.size());
    return *this;
  }
/*
#ifndef MUDUO_STD_STRING
  self& operator<<(const std::string& v)
  {
    buffer_.append(v.c_str(), v.size());
    return *this;
  }
#endif
*/

public:
  void append(const char* data, int len) { buffer_.append(data, len); }
  const Buffer& buffer() const { return buffer_; }
  void resetBuffer() { buffer_.reset(); }

private:
  void staticCheck();
  template<typename T>
  void formatInteger(T);

  Buffer buffer_;	//数据缓冲区
  static const int kMaxNumericSize = 32;  //数字最大位数
};


//用于将val格式化到buf_里
class Fmt // : boost::noncopyable
{
 public:
  template<typename T>
  Fmt(const char* fmt, T val);

  const char* data() const { return buf_; }
  int length() const { return length_; }

 private:
  char buf_[32];
  int length_;
};

inline LogStream& operator<<(LogStream& s, const Fmt& fmt) {
  s.append(fmt.data(), fmt.length());
  return s;
}

}
#endif  // MUDUO_BASE_LOGSTREAM_H