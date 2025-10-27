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
      ThreadWorkerExecutor executor(i, queue_);
      executor.Execute();
    });
  }
  LOG_DEBUG(fmt::format("{} threads started", kWorkerThreads));
}

void ThreadPool::Stop() {
  LOG_DEBUG("Stopping ThreadPool");

  for (auto& worker_thread : worker_threads_) {
    pthread_t thread_handle = worker_thread.native_handle();
    int result = pthread_cancel(thread_handle); // TODO: think of more safe thread cancellation
    if (result != 0) {
      LOG_ERROR(fmt::format("Error sending signal: {}", result));
    }
    worker_thread.join();
  }

  LOG_DEBUG(fmt::format("Number of uncompleted tasks in Thread Pool is {}", queue_.Size()));
}

ThreadPool::~ThreadPool() {}