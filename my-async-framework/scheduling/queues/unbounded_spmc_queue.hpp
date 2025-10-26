#pragma once

#include <deque>
#include <mutex>
#include <thread>

#include "../worker.hpp"

namespace MyAsyncFramework::scheduling::queues {

// Unbounded Single Producer Multiple Consumers Queue
template <typename Underlying = std::deque<Worker>> // TODO: think of adding concepts and multiple args templates
class UnboundedSpMcQueue {
public:
  UnboundedSpMcQueue() = default;

  UnboundedSpMcQueue(const UnboundedSpMcQueue&) = delete;
  UnboundedSpMcQueue(UnboundedSpMcQueue&&) = delete;
  UnboundedSpMcQueue& operator=(UnboundedSpMcQueue&&) = delete;
  UnboundedSpMcQueue& operator=(const UnboundedSpMcQueue&) = delete;

  ~UnboundedSpMcQueue() = default;

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