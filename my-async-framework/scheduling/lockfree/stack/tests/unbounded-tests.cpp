/*
cd "/home/ubuntu/recap/lockfree/stack/tests/"

clang++ -fsanitize=leak -std=c++20 unbounded-tests.cpp -o unbounded-tests.trash
clang++ -fsanitize=leak,address -std=c++20 unbounded-tests.cpp -o unbounded-tests.trash

"/home/ubuntu/recap/lockfree/stack/tests/"unbounded-tests.trash
*/
/*
clang++ -std=c++20 -O2 -g unbounded-tests.cpp -o unbounded-tests.trash
sudo perforator -e CPUClock record --serve :1234 -- ./unbounded-tests.trash
*/
#include "../unbounded-stack.hpp"

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
    UnboundedStack<int> stack;
    TestBasic(stack);
  }
  {
    UnboundedStack<int> stack;
    TestStressPushManyThreads(stack);
  }
  {
    UnboundedStack<int> stack;
    TestStressPopManyThreads(stack);
  }
  {
    UnboundedStack<int> stack;
    TestStressPushPopTogether(stack);
  }
}

void Benchmarks() {
  {
    UnboundedStack<int> stack;
    MeasureMs(TestStressPushPopTogether<UnboundedStack<int>>, stack);
  }
}

int main() {
  ReliabilityTests();
  while (true) {
    Benchmarks();  
  }
}