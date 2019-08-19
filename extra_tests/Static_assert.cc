#include <muduo/base/Timestamp.h>
using namespace muduo;

//编译时断言(assert为运行时断言)，如果argv[0]为真则什么也不做，否则输出argv[1]的提示
static_assert(sizeof(Timestamp) == sizeof(int64_t), "Timestamp is same size as int64_t");

//static_assert(sizeof(Timestamp) != sizeof(int64_t), "Timestamp is same size as int64_t");
//static_assert(sizeof(int) == sizeof(int64_t), "Timestamp is same size as int64_t");


int main() {
	return 0;
}