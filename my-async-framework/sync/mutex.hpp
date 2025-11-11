#pragma once

#include <my-async-framework/sync/atomic.hpp>
#include <my-async-framework/sync/futex_like.hpp>

namespace MyAsyncFramework::sync {

class Mutex {
public:
  void Lock() {
    uint32_t zero = 0u;

    if (!flag_.compare_exchange_strong(zero, 1u)) {
      while (flag_.exchange(2u) != 0u) {
        futex_wait(&flag_, 2u);
      }
    }
  }

  void Unlock() {
    if (flag_.exchange(0u) == 2u) {
      futex_wake(&flag_, 1);
    }
  }

  // BasicLockable https://en.cppreference.com/w/cpp/named_req/BasicLockable
  void lock() { Lock(); }
  void unlock() { Unlock(); }

private:
  // 0 - free
  // 1 - locked with no waiting threads
  // 2 - locked with waiting threads
  Atomic flag_{0};
};

/* 
--------------------------------------------------

The realization below is a naive but wrong one. It can get UB in case:
(https://gitlab.com/Lipovsky/concurrency-course/-/blob/master/tasks/sync/condvar/tests/event/storage.cpp?ref_type=heads)

class Event {
 public:
  void Wait() {
    std::unique_lock locker(mutex_);
    while (!ready_) {
      fire_.Wait(locker);
    }
  }

  void Fire() {
    std::lock_guard guard(mutex_);
    ready_ = true;
    fire_.NotifyOne();
  }

 private:
  bool ready_{false};
  Mutex mutex_;
  CondVar fire_;
};

TEST(Storage, 5s) {
  auto event = new Event{};

  twist::ed::std::thread t([event] {
    event->Fire();
  });

  event->Wait();
  delete event;

  t.join();
}

This happens due to deleted atomic call on line "futex_wake(&locked_, 1);"
The field mutex_ has been already destroyed before we're able to call futex wake
from the second thread. But the usage is still correct, the problem is that
we've "unlocked" our mutex at line "locked_.store(0);" but we can not wake
all the others simultaneously.
So in this file you can find pretty strange but at least correct realization of mutex


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

*/

} // namespace MyAsyncFramework::sync