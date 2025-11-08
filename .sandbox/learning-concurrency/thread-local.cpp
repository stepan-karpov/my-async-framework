#include <iostream>
#include <thread>

thread_local int thread_local_var = 0;

void foo() {
  for (int i = 0; i < 10'000; ++i) {
    ++thread_local_var;
  }
  std::cout << thread_local_var << "\n";
}

int main() {
  std::thread t1(foo);
  std::thread t2(foo);
  std::thread t3(foo);
  std::thread t4(foo);

  t1.join(); t2.join(); t3.join(); t4.join();
}