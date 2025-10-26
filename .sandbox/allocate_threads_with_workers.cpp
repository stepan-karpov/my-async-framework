#include <iostream>
#include <thread>
#include <atomic>

struct Worker {
  Worker(std::atomic<bool>& need_to_sleep) : need_to_sleep_(need_to_sleep) {}

  void operator()() {
    while (need_to_sleep_.load()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "I am foo\n";
  }

  std::atomic<bool>& need_to_sleep_;
};

int main() {
  std::atomic<bool> need_to_sleep;
  need_to_sleep.store(true);

  std::thread t1((Worker(need_to_sleep)));
  std::thread t2((Worker(need_to_sleep)));

  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  need_to_sleep.store(false);
  
  t1.join();
  t2.join();
}