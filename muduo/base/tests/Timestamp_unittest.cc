#include <muduo/base/Timestamp.h>
#include <vector>
#include <stdio.h>

using muduo::Timestamp;

void passByConstReference(const Timestamp& x)
{
  printf("%s\n", x.toString().c_str());
}

void passByValue(Timestamp x)
{
  printf("%s\n", x.toString().c_str());
}

void benchmark()
{
  const int kNumber = 1000 * 10000;

  std::vector<Timestamp> stamps;
  stamps.reserve(kNumber);

  printf("Create %d clocks......\n", kNumber);
  for (int i = 0; i < kNumber; ++i)
  {/* 创建 1000 * 10000 个时钟对象 */
    stamps.push_back(Timestamp::now());
  }
  printf("Create first clock at: %s s\n", stamps.front().toString().c_str());
  printf("Create last  clock at: %s s\n", stamps.back().toString().c_str());
  printf("Time difference of (last - first): %f s\n", timeDifference(stamps.back(), stamps.front()));
  printf("-----------------------------------------------------------\n\n");

  int increments[100] = { 0 };
  int64_t start = stamps.front().microSecondsSinceEpoch();
  for (int i = 1; i < kNumber; ++i)
  {
    int64_t next = stamps[i].microSecondsSinceEpoch();
    int64_t inc = next - start;
    start = next;
    if (inc < 0)
    {
      printf("reverse!\n");
    }
    else if (inc < 100)
    {
      ++increments[inc];
    }
    else
    {
      printf("big gap %d us\n", static_cast<int>(inc));
    }
  }

  printf("this-last(us)      count\n");
  for (int i = 0; i < 100; ++i)
  {
    printf("    %2d    %14d\n", i, increments[i]);
  }
}



int main()
{
  Timestamp now(Timestamp::now());		//得到记录当前时间的对象
  
  printf("Today is: %s\n\n", now.toFormattedString(true).c_str());
  printf("%s\n", now.toString().c_str());  
  passByValue(now);
  passByConstReference(now);
  
  printf("-----------------------------------------------------------\n\n");
  benchmark();
}

