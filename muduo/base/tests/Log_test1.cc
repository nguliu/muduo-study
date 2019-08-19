#include <muduo/base/Logging.h>
#include <errno.h>
#include <string>

using namespace muduo;

int main()
{
	Logger::setLogLevel(Logger::TRACE);
	
	LOG_TRACE<<"trace...";
	LOG_DEBUG<<"debug...";
	LOG_INFO<<"info...";
	LOG_WARN<<"warn...";
	LOG_ERROR<<"error...";
	//LOG_FATAL<<"fatal ...";
	errno = 1;
	LOG_SYSERR<<"syserr...";
	//LOG_SYSFATAL<<"sysfatal...";
	return 0;
}
