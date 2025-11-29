// cd "/home/ubuntu/my-async-framework/.sandbox/learning-concurrency/" &&  clang++ -std=c++20 "spinlock.cpp" -o "spinlock.trash" &&  ./"spinlock.trash"

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>

inline void SpinLockPause() {
  asm volatile("pause\n" ::: "memory");
}

class Spinlock {
public:
  void Lock() {
    while (lock_.exchange(1, std::memory_order::acquire) == 1) {
      while (lock_.load(std::memory_order::relaxed)) {
        SpinLockPause();
      }
    }
    // while (lock_.exchange(1) == 1) {
    //   SpinLockPause();
    // }
  }

  void Unlock() {
    lock_.store(0, std::memory_order::release);
    // lock_.store(0);
  }

private:
  std::atomic<size_t> lock_{0};
};


void Stress() {
  auto start = std::chrono::high_resolution_clock::now();

  int common = 0;
  Spinlock lock;

  std::vector<std::thread> threads;
  for (int i = 0; i < 2; ++i) {
    threads.emplace_back([&lock, &common] {
      for (int j = 0; j < 10'000'000; ++j) {
        lock.Lock();
        ++common;
        lock.Unlock();
      }
    });
  }

  for (auto& t : threads) {
    t.join();
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  std::cout << "Time: " << duration.count() << " ms\n";
  std::cout << "Common: " << common << "\n";
}

int main() {
  while (true) {
    Stress();
  }
}