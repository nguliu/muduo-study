#include <stdio.h>
#include <vector>
using namespace std;
#pragma GCC diagnostic ignored "-Wold-style-cast"	//暂时忽略C风格的强制类型转换

int main() {
	vector<int> vec1{1, 2, 3};
	vector<int> vec2{4, 5, 6, 7, 8, 9};

	printf("交换前vec1: start = %p size = %d capacity = %d  内容:",
			&*vec1.begin(), (int)vec1.size(), (int)vec1.capacity());
	for (auto x : vec1) {
		printf(" %d", x);
	}
	printf("\n交换前vec2: start = %p size = %d capacity = %d  内容:",
					&*vec2.begin(), (int)vec2.size(), (int)vec2.capacity());
	for (auto x : vec2) {
		printf(" %d", x);
	}

	vec1.swap(vec2);
	printf("\n\n交换后vec1: start = %p size = %d capacity = %d  内容:",
			&*vec1.begin(), (int)vec1.size(), (int)vec1.capacity());
	for (auto x : vec1) {
		printf(" %d", x);
	}
	printf("\n交换后vec2: start = %p size = %d capacity = %d  内容:",
					&*vec2.begin(), (int)vec2.size(), (int)vec2.capacity());
	for (auto x : vec2) {
		printf(" %d", x);
	}

	vec1.resize(100);
	printf("\n\nvec1.resize(100)后: start = %p size = %d capacity = %d  内容:",
			&*vec1.begin(), (int)vec1.size(), (int)vec1.capacity());
	for (auto x : vec1) {
		printf(" %d", x);
	}
	printf("\n\n");

	return 0;
}