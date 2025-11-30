#pragma once

#include <mutex>

#include <my-async-framework/sync/condition_variable.hpp>
#include <my-async-framework/sync/mutex.hpp>
#include <my-async-framework/sync/atomic.hpp>

namespace MyAsyncFramework::sync {

class WaitGroup {
 public:
  void Add(size_t count) {
    counter_.fetch_add(count);
  }

  void Done() {
    std::unique_lock lock(mutex_);
    counter_.fetch_sub(1);
    if (counter_ == 0 && waiters_ > 0) {
      cv_.NotifyAll();
    }
  }

  void Wait() {
    std::unique_lock lock(mutex_);
    ++waiters_;
    while (counter_.load() != 0) {
      cv_.Wait(lock);
    }
    --waiters_;
  }

 private:
  MyAsyncFramework::sync::Atomic counter_{0};
  int64_t waiters_{0};

  MyAsyncFramework::sync::ConditionVariable cv_;
  MyAsyncFramework::sync::Mutex mutex_;
};

} // namespace MyAsyncFramework::sync