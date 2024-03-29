// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef MUDUO_NET_HTTP_HTTPREQUEST_H
#define MUDUO_NET_HTTP_HTTPREQUEST_H

#include <muduo/base/copyable.h>
#include <muduo/base/Timestamp.h>
#include <muduo/base/Types.h>

#include <map>
#include <assert.h>
#include <stdio.h>

namespace muduo
{
namespace net
{
// http请求信息类封装
class HttpRequest : public muduo::copyable
{
 public:
  enum Method { //请求方法
    kInvalid, kGet, kPost, kHead, kPut, kDelete
  };
  enum Version {	// 协议版本
    kUnknown = -1,
	kHttp10 = 0,
	kHttp11 = 1,
  };


  HttpRequest()
    : method_(kInvalid),
      version_(kUnknown)
  { }

  void setVersion(Version v) { version_ = v; }
  
  Version getVersion() const { return version_; }
  
  string getVersionString() const {
	string v;
	if (version_ == kHttp10) {
		v = "HTTP/1.0";
	}
	else if (version_ == kHttp11) {
		v = "HTTP/1.1";
	}
	return v;
  }

  bool setMethod(const char* start, const char* end)
  {
    assert(method_ == kInvalid);
	
    string m(start, end);
    if (m == "GET")
    {
      method_ = kGet;
    }
    else if (m == "POST")
    {
      method_ = kPost;
    }
    else if (m == "HEAD")
    {
      method_ = kHead;
    }
    else if (m == "PUT")
    {
      method_ = kPut;
    }
    else if (m == "DELETE")
    {
      method_ = kDelete;
    }
    else
    {
      method_ = kInvalid;
    }
    return method_ != kInvalid;
  }

  Method method() const { return method_; }
  
  const char* methodString() const
  {
    const char* result = "UNKNOWN";
    switch(method_)
    {
      case kGet:
        result = "GET";
        break;
      case kPost:
        result = "POST";
        break;
      case kHead:
        result = "HEAD";
        break;
      case kPut:
        result = "PUT";
        break;
      case kDelete:
        result = "DELETE";
        break;
      default:
        break;
    }
    return result;
  }

  void setPath(const char* start, const char* end)
  { path_.assign(start, end); }

  const string& path() const
  { return path_; }

  void setReceiveTime(Timestamp t)
  { receiveTime_ = t; }

  Timestamp receiveTime() const
  { return receiveTime_; }

  void addHeader(const char* start, const char* colon, const char* end)
  {
    string field(start, colon);		// header域
    ++colon;
    // 去除左空格
    while (colon < end && isspace(*colon))
    {
      ++colon;
    }
    string value(colon, end);		// header值
    // 去除右空格
    while (!value.empty() && isspace(value[value.size()-1]))
    {
      value.resize(value.size()-1);
    }
    headers_[field] = value;
  }

  string getHeader(const string& field) const
  {
    string result;
    std::map<string, string>::const_iterator it = headers_.find(field);
    if (it != headers_.end())
    {
      result = it->second;
    }
    return result;
  }

  const std::map<string, string>& headers() const
  { return headers_; }

  void swap(HttpRequest& that)
  {
    std::swap(method_, that.method_);
	std::swap(version_, that.version_);
    path_.swap(that.path_);
    receiveTime_.swap(that.receiveTime_);
    headers_.swap(that.headers_);
  }

 private:
  Method method_;			// 请求方法
  string path_;				// 请求路径
  Version version_;			// 协议版本1.0/1.1
  std::map<string, string> headers_;  // header列表
  
  Timestamp receiveTime_;	// 请求时间（收到请求的时间）
};

}
}

#endif  // MUDUO_NET_HTTP_HTTPREQUEST_H
