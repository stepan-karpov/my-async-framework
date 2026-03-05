#pragma once

#include <my-async-framework/logging/logging.hpp>
#include <my-async-framework/sync/queues/unbounded_mpmc_queue.hpp>

#include "thread_pool_fwd.hpp"
#include "worker.hpp"

namespace MyAsyncFramework::scheduling::fiber::thread_pool {

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

} // namespace MyAsyncFramework::scheduling::fiber::thread_pool

