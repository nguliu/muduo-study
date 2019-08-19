// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is an internal header file, you should not include this.

#ifndef MUDUO_NET_HTTP_HTTPCONTEXT_H
#define MUDUO_NET_HTTP_HTTPCONTEXT_H

#include <muduo/base/copyable.h>

#include <muduo/base/Timestamp.h>
#include <muduo/net/http/HttpRequest.h>

namespace muduo
{
namespace net
{

class Buffer;

//http协议解析类封装
class HttpContext : public muduo::copyable
{
 public:
  enum HttpRequestParseState
  {
    kExpectRequestLine, //正在解析请求行
    kExpectHeaders,		//正在解析头部（请求行解析成功）
    kExpectBody,		//正在解析实体（请求行和头部解析成功）
    kGotAll,			//全部解析成功
  };


  HttpContext()	// 默认处于请求行状态
    : state_(kExpectRequestLine)
  { }

  // default copy-ctor, dtor and assignment are fine

  bool expectRequestLine() const
  { return state_ == kExpectRequestLine; }

  bool expectHeaders() const
  { return state_ == kExpectHeaders; }

  bool expectBody() const
  { return state_ == kExpectBody; }

  bool gotAll() const
  { return state_ == kGotAll; }

  void receiveRequestLine() //请求行解析成功
  { state_ = kExpectHeaders; }

  void receiveHeaders() 		  //头部解析成功
  { state_ = kGotAll; }  // FIXME

  // 重置HttpContext状态
  void reset()
  {
    state_ = kExpectRequestLine;
    HttpRequest dummy;
    request_.swap(dummy);
  }

  const HttpRequest& request() const
  { return request_; }

  HttpRequest& request()
  { return request_; }

  //解析[begin, end)是否是正确的请求行
  bool processRequestLine(const char* begin, const char* end);
  
  //解析buf内的消息
  bool parseRequest(Buffer* buf, muduo::Timestamp receiveTime);

 private:
  HttpRequestParseState state_;		// 请求解析状态
  HttpRequest request_;				// http请求信息
};

}
}

#endif  // MUDUO_NET_HTTP_HTTPCONTEXT_H
