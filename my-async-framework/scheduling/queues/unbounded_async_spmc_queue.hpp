#pragma once

#include <deque>
#include <mutex>
#include <thread>

#include "../worker.hpp"

namespace MyAsyncFramework::scheduling::queues {

/*
Unbounded Sync Single Producer Multiple Consumers Queue
Sync means that queue sleeps (with sleep(10ms)) in PopFront until queue is not empty
*/
template <typename Underlying = std::deque<Worker>> // TODO: think of adding concepts and multiple args templates
class UnboundedAsyncSpMcQueue {
public:
  UnboundedAsyncSpMcQueue() = default;

  UnboundedAsyncSpMcQueue(const UnboundedAsyncSpMcQueue&) = delete;
  UnboundedAsyncSpMcQueue(UnboundedAsyncSpMcQueue&&) = delete;
  UnboundedAsyncSpMcQueue& operator=(UnboundedAsyncSpMcQueue&&) = delete;
  UnboundedAsyncSpMcQueue& operator=(const UnboundedAsyncSpMcQueue&) = delete;

  ~UnboundedAsyncSpMcQueue() = default;

  void PushBack(Worker&& f) {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    underlying_container_.push_back(std::move(f));
  }

  // Blocking method!
  Worker PopFront() {
    std::unique_lock<std::mutex> unique_lock(mutex_);
    bool is_container_empty = underlying_container_.empty();

    while (is_container_empty) {
      unique_lock.unlock();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      unique_lock.lock();
      is_container_empty = underlying_container_.empty();
    }

    auto front_value = std::move(underlying_container_.front());
    underlying_container_.pop_front();
    return front_value;
  }

  size_t Size() { return underlying_container_.size(); }
private:
  std::mutex mutex_;
  Underlying underlying_container_;
};

} // namespace MyAsyncFramework::scheduling::Queue