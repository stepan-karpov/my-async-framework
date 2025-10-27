#pragma once

#include "queues/unbounded_sync_spmc_queue.hpp"

#include <thread>
#include <iostream>

#include <my-async-framework/logging/logging.hpp>

namespace MyAsyncFramework::scheduling {

using Queue = queues::UnboundedSyncSpMcQueue<>;

class ThreadWorkerExecutor {
public:
  ThreadWorkerExecutor(const int number, Queue& queue, std::atomic<bool>& should_be_stopped)
    : thread_pool_queue_(queue),
      should_be_stopped_(should_be_stopped),
      kMyNumberAsAThread_(number) {
    LOG_DEBUG(fmt::format("Executor {} start executing", number));
  }

  void Execute() {
    while (!should_be_stopped_.load()) {
      auto next_worker = thread_pool_queue_.PopFront();
      LOG_DEBUG(fmt::format("ThreadWorkerExecutor number {} executing his task", kMyNumberAsAThread_));
      next_worker();
    }
    LOG_DEBUG(fmt::format("ThreadWorkerExecutor number {} stopped", kMyNumberAsAThread_));
  }
private:
  Queue& thread_pool_queue_;
  std::atomic<bool>& should_be_stopped_;
  const int kMyNumberAsAThread_;
};

class ThreadPool {
public:
  ThreadPool();
  ThreadPool(const int worker_threads);
  ~ThreadPool();

  void AddTask(Worker&& worker) {
    queue_.PushBack(std::move(worker));
  }
  void Stop();
private:
  void AllocateAndStartWorkerThreads();

  Queue queue_;
  std::vector<std::thread> worker_threads_;
  std::atomic<bool> should_be_stopped_;

  const int kWorkerThreads = 4;
};

} // namespace MyAsyncFramework::scheduling

