#pragma once

#include <deque>
#include <mutex>

#include <my-async-framework/logging/logging.hpp>
#include <my-async-framework/sync/mutex.hpp>
#include <my-async-framework/sync/condition_variable.hpp>

namespace MyAsyncFramework::sync::queues {

/*
Unbounded Multiple Producers Multiple Consumers Queue
*/
 // TODO: add std::deque as a template parameter with multiple templates
template <typename T>
class UnboundedMpMcQueue {
public:
  UnboundedMpMcQueue() = default;

  UnboundedMpMcQueue(const UnboundedMpMcQueue&) = delete;
  UnboundedMpMcQueue(UnboundedMpMcQueue&&) = delete;
  UnboundedMpMcQueue& operator=(UnboundedMpMcQueue&&) = delete;
  UnboundedMpMcQueue& operator=(const UnboundedMpMcQueue&) = delete;

  ~UnboundedMpMcQueue() = default;

  template <typename U> // TODO: add concept
  void PushBack(U&& f) {
    std::unique_lock<MyAsyncFramework::sync::Mutex> lock(mutex_);
    if (is_closed) {
      LOG_ERROR("You're trying to push a value to already closed UnboundedMpMcQueue. Value will be skipped\n");
      return;
    }
    underlying_container_.push_back(std::forward<U>(f));
    not_empty_.NotifyOne();
  }

  // Blocking method!
  // std::nullopt when a queue has already been closed
  std::optional<T> PopFront() {
    std::unique_lock<MyAsyncFramework::sync::Mutex> lock(mutex_);
    while (underlying_container_.empty() && !is_closed) {
      not_empty_.Wait(lock);
    }
    if (underlying_container_.empty()) {
      return std::nullopt;
    }
    auto front_value = std::move(underlying_container_.front());
    underlying_container_.pop_front();
    return front_value;
  }

  void Close() {
    std::unique_lock<MyAsyncFramework::sync::Mutex> lock(mutex_);
    is_closed = true;
    not_empty_.NotifyAll();
  }

  // for info only
  size_t Size() { return underlying_container_.size(); }
private:
  bool is_closed = false;
  MyAsyncFramework::sync::Mutex mutex_;
  MyAsyncFramework::sync::ConditionVariable not_empty_;
  std::deque<T> underlying_container_;
};

} // namespace MyAsyncFramework::sync::queues