#include <iostream>
#include <thread>
#include <chrono>

std::mutex mutex;
std::condition_variable cv;
bool can_continue = 0;

void foo(const int num) {
  std::unique_lock lock(mutex);
  // while (!can_continue) {
  //   cv.wait(lock);
  // }
  cv.wait(lock, [] { return can_continue; });

  std::cout << "Hello world from foo " << num << "\n";
}

void bar() {
  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "Hello world from bar\n";
  can_continue = true;
  std::cout << "can_continue = true\n";
  cv.notify_one();
  std::this_thread::sleep_for(std::chrono::seconds(3));
  cv.notify_one();
}

int main() {
  std::thread t1(foo, 1);
  std::thread t2(foo, 2);
  std::thread t3(bar);

  t1.join();
  t2.join();
  t3.join();
}