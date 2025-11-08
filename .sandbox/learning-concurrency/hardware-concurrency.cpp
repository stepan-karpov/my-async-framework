#include <iostream>
#include <thread>

int main() {
  auto n = std::thread::hardware_concurrency();
  std::cout << "std::thread::hardware_concurrency() = " << n << "\n";

  // std::thread::hardware_concurrency() = 10
}