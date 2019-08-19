// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//

#include <muduo/net/http/HttpServer.h>

#include <muduo/base/Logging.h>
#include <muduo/net/http/HttpContext.h>
#include <muduo/net/http/HttpRequest.h>
#include <muduo/net/http/HttpResponse.h>

#include <boost/bind.hpp>

using namespace muduo;
using namespace muduo::net;

namespace muduo
{
namespace net
{
namespace detail
{
  void defaultHttpCallback(const HttpRequest&, HttpResponse* response)
  {
    response->setStatusCode(HttpResponse::k404NotFound);
    response->setStatusMessage("Not Found");
    response->setCloseConnection(true);
  }
}
}
}


HttpServer::HttpServer(EventLoop* loop,
                       const InetAddress& listenAddr,
                       const string& name)
  : server_(loop, listenAddr, name),
    httpCallback_(detail::defaultHttpCallback)
{
  server_.setConnectionCallback(
      boost::bind(&HttpServer::onConnection, this, _1));
  server_.setMessageCallback(
      boost::bind(&HttpServer::onMessage, this, _1, _2, _3));
}

HttpServer::~HttpServer()
{
}

void HttpServer::start()
{
  LOG_INFO << "HttpServer [" << server_.name()
    << "] starts listenning on port " << server_.hostport();
  server_.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn)
{
  if (conn->connected())
  {
    conn->setContext(HttpContext());	// 为每一个新连接绑定一个Http请求解析类对象
  }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn,
                           Buffer* buf,
                           Timestamp receiveTime)
{
  //取出当前连接的Http请求解析类对象
  HttpContext* context = boost::any_cast<HttpContext>(conn->getMutableContext());

  if (!context->parseRequest(buf, receiveTime)) //解析当前请求
  {
    conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
    conn->shutdown();
  }

  // 请求消息解析完毕
  if (context->gotAll())
  {
    onRequest(conn, context->request());
    context->reset();  // 本次请求处理完毕，重置HttpContext，适用于长连接
  }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& request)
{
  const string& connection = request.getHeader("Connection");
  bool close = connection == "close" ||
    (request.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
  
  HttpResponse response(close);		 //构造一个HTTP响应对象
  httpCallback_(request, &response); //调用外部用户回调函数处理这个请求（response是传出参数）
  
  Buffer buf;
  response.appendToBuffer(&buf);
  conn->send(&buf);
  
  if (response.closeConnection()) {  //如果是短连接需要关闭
    conn->shutdown();
  }
}

