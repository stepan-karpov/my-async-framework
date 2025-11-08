#pragma once

#include <my-async-framework/sync/atomic.hpp>
#include <my-async-framework/sync/futex_like.hpp>

namespace MyAsyncFramework::sync {

class ConditionVariable {
public:
  template <class Mutex>
  void Wait(Mutex& mutex) { // TODO: add concept for "lock + unlock"
    counter_.fetch_add(1);
    int value = counter_.load();

    mutex.unlock();
    futex_wait(&counter_, value);
    mutex.lock();
  }

  void NotifyOne() {
    counter_.fetch_add(1);
    futex_wake(&counter_, 1);
  }

  void NotifyAll() {
    counter_.fetch_add(1);
    futex_wake_all(&counter_);
  }

private:
  Atomic counter_{0};
};

} // namespace MyAsyncFramework::sync
