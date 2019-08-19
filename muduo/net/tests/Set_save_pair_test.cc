#include <stdio.h>
#include <set>
#include <utility>
using namespace std;
#pragma GCC diagnostic ignored "-Wold-style-cast"	//暂时忽略C风格的强制类型转换

typedef pair<int, int*> Pair;

int main() {
    int array[5] = {0, 1, 2, 3, 4};
    set<Pair> ipset;

    ipset.insert(Pair(array[1], &array[4]));
    ipset.insert(Pair(array[2], &array[2]));
    ipset.insert(Pair(array[2], &array[1]));
    ipset.insert(Pair(array[2], &array[3]));
    ipset.insert(Pair(array[3], &array[0]));
	
	for (auto it = ipset.begin(); it != ipset.end(); ++it) {
		printf("it.first = %d, it.second = %p\n", it->first, it->second);
	}

	return 0;
}
