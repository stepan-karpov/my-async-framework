#pragma once

#include <thread>
#include <iostream>

#include <my-async-framework/logging/logging.hpp>

#include <my-async-framework/scheduling/worker.hpp>
#include <my-async-framework/sync/queues/unbounded_mpmc_queue.hpp>

namespace MyAsyncFramework::scheduling {

using Queue = MyAsyncFramework::sync::queues::UnboundedMpMcQueue<Worker>;

class ThreadWorkerExecutor {
public:
  ThreadWorkerExecutor(const int number, Queue& queue)
    : thread_pool_queue_(queue),
      kMyNumberAsAThread_(number) {
    LOG_DEBUG(fmt::format("Executor {} start executing", number));
  }

  void Execute() {
    while (true) {
      auto next_task = thread_pool_queue_.PopFront();
      if (!next_task.has_value()) {
        break;
      }
      LOG_DEBUG(fmt::format("ThreadWorkerExecutor number {} executing his task", kMyNumberAsAThread_));
      next_task.value()(); // TODO: think of exceptions?
    }
    LOG_DEBUG(fmt::format("ThreadWorkerExecutor number {} stopped", kMyNumberAsAThread_));
  }
private:
  Queue& thread_pool_queue_;
  const int kMyNumberAsAThread_;
};

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

} // namespace MyAsyncFramework::scheduling

