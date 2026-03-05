#pragma once

#include <thread>
#include <iostream>

#include <my-async-framework/logging/logging.hpp>
#include <my-async-framework/sync/queues/unbounded_mpmc_queue.hpp>

#include "worker.hpp"

namespace MyAsyncFramework::scheduling::fiber::thread_pool {

using Queue = MyAsyncFramework::sync::queues::UnboundedMpMcQueue<Worker>;

class ThreadPool {
public:
  ThreadPool();
  ThreadPool(const int worker_threads);
  ~ThreadPool();

  void AddTask(Worker&& worker);
  void Stop();
  void Start();

private:
  bool is_started = false;
  bool is_stopped_ = false;
  Queue queue_;
  std::vector<std::thread> worker_threads_;

  const int kWorkerThreads = 8;
};

} // namespace MyAsyncFramework::scheduling::fiber::thread_pool

