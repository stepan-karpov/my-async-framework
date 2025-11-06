#pragma once

#include <my-async-framework/sync/atomic.hpp>
#include <my-async-framework/sync/futex_like.hpp>

namespace MyAsyncFramework::sync {

class Mutex {
public:
  void Lock() {
    while (locked_.exchange(1) == 1) {
      blocked_number_.fetch_add(1);
      futex_wait(&locked_, 1);
      blocked_number_.fetch_sub(1);
    }
  }

  void Unlock() {
    locked_.store(0);
    if (blocked_number_.load()) {
      futex_wake(&locked_, 1);
    }
  }

  // BasicLockable https://en.cppreference.com/w/cpp/named_req/BasicLockable
  void lock() { Lock(); }
  void unlock() { Unlock(); }

private:
  Atomic locked_{0};
  Atomic blocked_number_{0}; // For no needless wake up 
};

} // namespace MyAsyncFramework::sync