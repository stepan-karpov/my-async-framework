/*
cd "/home/ubuntu/recap/lockfree/stack/tests/"

clang++ -fsanitize=leak -std=c++20 bounded-tests.cpp -o bounded-tests.trash
clang++ -fsanitize=leak,address -std=c++20 bounded-tests.cpp -o bounded-tests.trash

"/home/ubuntu/recap/lockfree/stack/tests/"bounded-tests.trash
*/
/*
clang++ -std=c++20 -O2 -g bounded-tests.cpp -o bounded-tests.trash
sudo perforator -e CPUClock record --serve :1234 -- ./bounded-tests.trash
*/
#include "../bounded-stack.hpp"

#include "common/stress.hpp"
#include "common/unit.hpp"

template <typename Stack>
void MeasureMs(auto function, Stack& stack) {
  const auto t0 = std::chrono::steady_clock::now();
  function(stack);
  const auto t1 = std::chrono::steady_clock::now();
  const auto ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
  std::cout << "Time spent: " << ms << " ms\n";
}

void ReliabilityTests() {
  {
    BoundedStack<int> stack(4);
    TestBasic(stack);
  }
  {
    BoundedStack<int> stack(4);
    TestBasicBounded(stack);
  }
  {
    BoundedStack<int64_t> stack(10'000'000);
    TestStressPushManyThreads(stack);
  }
  {
    BoundedStack<int64_t> stack(10'000'000);
    TestStressPopManyThreads(stack);
  }
  {
    BoundedStack<int64_t> stack(10'000'000);
    TestStressPushPopTogether(stack);
  }
}

void Benchmarks() {
  {
    BoundedStack<int64_t> stack(5'000'000);
    MeasureMs(TestStressPushPopTogether<BoundedStack<int64_t>>, stack);
  }
}

int main() {
  ReliabilityTests();
  while (true) {
    Benchmarks();
  }
}