#pragma once

#include <deque>
#include <mutex>
#include <thread>
#include <condition_variable>

#include <my-async-framework/sync/mutex.hpp>

#include "../worker.hpp"

namespace MyAsyncFramework::scheduling::queues {

/*
Bounded Sync Single Producer Multiple Consumers Queue
Sync means that queue sleeps (with sleep(10ms)) in PopFront until queue is not empty
*/
// TODO: fix naming
template <typename Underlying = std::deque<Worker>, size_t kMaxQueueSize = 100> // TODO: think of adding concepts and multiple args templates
class BoundedAsyncSpMcQueue {
public:
  BoundedAsyncSpMcQueue() = default;

  BoundedAsyncSpMcQueue(const BoundedAsyncSpMcQueue&) = delete;
  BoundedAsyncSpMcQueue(BoundedAsyncSpMcQueue&&) = delete;
  BoundedAsyncSpMcQueue& operator=(BoundedAsyncSpMcQueue&&) = delete;
  BoundedAsyncSpMcQueue& operator=(const BoundedAsyncSpMcQueue&) = delete;

  ~BoundedAsyncSpMcQueue() = default;

  void PushBack(Worker&& f) {
    std::lock_guard<MyAsyncFramework::sync::Mutex> lock_guard(mutex_);
    not_full_.wait(lock, [this]{
      return underlying_container_.size() < kMaxQueueSize
    });
    underlying_container_.push_back(std::move(f));
    not_empty_.notify_one();
  }

  // Blocking method!
  Worker PopFront() {
    std::unique_lock<MyAsyncFramework::sync::Mutex> unique_lock(mutex_);
    not_empty_.wait(lock, [this]{
      return underlying_container_.size() > 0
    });
    auto front_value = std::move(underlying_container_.front());
    underlying_container_.pop_front();
    not_full_.notify_one();
    return front_value;
  }

  size_t Size() { return underlying_container_.size(); }
private:
  MyAsyncFramework::sync::Mutex mutex_;
  std::condition_variable not_empty_;
  std::condition_variable not_full_;
  Underlying underlying_container_;
};

} // namespace MyAsyncFramework::scheduling::Queue