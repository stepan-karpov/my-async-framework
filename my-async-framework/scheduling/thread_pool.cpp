#include "thread_pool.hpp"

#include <my-async-framework/logging/logging.hpp>

#include <iostream>

using namespace MyAsyncFramework::scheduling;

ThreadPool::ThreadPool() {
  LOG_DEBUG("ThreadPool constructed");
  AllocateAndStartWorkerThreads();
}

ThreadPool::ThreadPool(const int worker_threads) : kWorkerThreads(worker_threads) {
  LOG_DEBUG("ThreadPool constructed");
  AllocateAndStartWorkerThreads();
}


void ThreadPool::AllocateAndStartWorkerThreads() {
  worker_threads_.reserve(kWorkerThreads);
  for (size_t i = 0; i < kWorkerThreads; ++i) {
    worker_threads_.emplace_back([this, i]() {
      ThreadWorkerExecutor executor(i, queue_, should_be_stopped_);
      executor.Execute();
    });
  }
  LOG_DEBUG(fmt::format("{} threads started", kWorkerThreads));
}

void ThreadPool::Stop() {
  LOG_DEBUG("Stopping ThreadPool");
  should_be_stopped_.store(true);

  for (auto& worker_thread : worker_threads_) {
    worker_thread.join();
  }

  LOG_DEBUG(fmt::format("Number of uncompleted tasks in Thread Pool is {}", queue_.Size()));
}

ThreadPool::~ThreadPool() {
  if (!should_be_stopped_.load()) {
    Stop();
  }
}