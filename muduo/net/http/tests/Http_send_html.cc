#include <muduo/net/http/HttpServer.h>
#include <muduo/net/http/HttpRequest.h>
#include <muduo/net/http/HttpResponse.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>

#include <iostream>
#include <map>
#include <string>

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

std::string fileContent;

void readFile(const char* path) {
	fileContent.clear();

	int fd= open(path, O_RDONLY);
	if (fd >= 0) {
		char buf[1024];
		while (::read(fd, buf, sizeof buf) > 0) {
			fileContent += buf;
		}
	}
	else {
		fileContent += "<html><head><title>Welcome to my home page</title></head>"
        "<body><h1>File is not finded</h1></body></html>";
	}
}

void responseInit(HttpResponse* response) {
	response->setStatusCode(HttpResponse::k200Ok);
    response->setStatusMessage("OK");
    response->setContentType("text/html");
    response->addHeader("Server", "MuduoHttpServer");
}

// 实际的请求处理
void onRequest(const HttpRequest& request, HttpResponse* response)
{
  std::cout << request.methodString() << " " << request.path() << " " << request.getVersionString() << std::endl;

  const std::map<string, string>& headers = request.headers();
  for (std::map<string, string>::const_iterator it = headers.begin();
  	 it != headers.end();
	 ++it)
  {
    std::cout << it->first << ": " << it->second << std::endl;
  }

  response->setVersion(static_cast<HttpResponse::Version>(request.getVersion()));
  if (request.path() == "/")
  {
	responseInit(response);
    string now = Timestamp::now().toFormattedString();
    response->setBody("<html><head><title>View time</title></head>"
        "<body><h1>Hello</h1>Now is " + now +
        "</body></html>");
  }
  else if (request.path() == "/grade")
  {
	responseInit(response);
	readFile("/home/html/GradeCalculate.html");
    response->setBody(fileContent);
  }
  else if (request.path() == "/wangyi")
  {
	responseInit(response);
	readFile("/home/html/wangyi/main.html");
    response->setBody(fileContent);
  }
  else if (request.path() == "/top.html")
  {
	responseInit(response);
	readFile("/home/html/wangyi/top.html");
    response->setBody(fileContent);
  }
  else if (request.path() == "/bottom1.html")
  {
	responseInit(response);
	readFile("/home/html/wangyi/bottom1.html");
    response->setBody(fileContent);
  }
  else if (request.path() == "/bottom2.html")
  {
	responseInit(response);
	readFile("/home/html/wangyi/bottom2.html");
    response->setBody(fileContent);
  }
  else if (request.path() == "/bottom3.html")
  {
	responseInit(response);
	readFile("/home/html/wangyi/bottom3.html");
    response->setBody(fileContent);
  }
  else if (request.path() == "/bottom2Div.css")
  {
	response->setStatusCode(HttpResponse::k200Ok);
    response->setStatusMessage("OK");
    response->setContentType("text/css");
    response->addHeader("Server", "MuduoHttpServer");
	readFile("/home/html/wangyi/bottom2Div.css");
    response->setBody(fileContent);
  }
  else if (request.path() == "/DingLei.jpg")
  {
	response->setStatusCode(HttpResponse::k200Ok);
    response->setStatusMessage("OK");
    response->setContentType("image/jpg");
    response->addHeader("Server", "MuduoHttpServer");
	readFile("/home/html/wangyi/DingLei.jpg");
    response->setBody(fileContent);
  }
  else
  {
    response->setStatusCode(HttpResponse::k404NotFound);
    response->setStatusMessage("Not Found");
    response->setCloseConnection(true);
  }
}


int main(int argc, char* argv[])
{
  EventLoop loop;
  HttpServer httpServer(&loop, InetAddress(8000), "HttpServer");
  httpServer.setHttpCallback(onRequest);
  httpServer.setThreadNum(0);
  httpServer.start();
  loop.loop();
}