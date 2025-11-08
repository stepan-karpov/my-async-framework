#include "thread_pool.hpp"

#include <my-async-framework/logging/logging.hpp>

#include <iostream>

using namespace MyAsyncFramework::scheduling;

ThreadPool::ThreadPool() {
  LOG_DEBUG("ThreadPool constructed");
}

ThreadPool::ThreadPool(const int worker_threads) : kWorkerThreads(worker_threads) {
  LOG_DEBUG("ThreadPool constructed");
}

void ThreadPool::Start() {
  worker_threads_.reserve(kWorkerThreads);
  for (size_t i = 0; i < kWorkerThreads; ++i) {
    worker_threads_.emplace_back([this, i]() {
      ThreadWorkerExecutor executor(i, queue_);
      executor.Execute();
    });
  }
  is_started = true;
  LOG_DEBUG(fmt::format("{} threads started", kWorkerThreads));
}

void ThreadPool::Stop() {
  if (!is_started) {
    LOG_ERROR("ThreadPool can not be stopped since it wasn't started\n");
    throw std::runtime_error("ThreadPool can not be stopped since it wasn't started\n");
  }

  LOG_DEBUG("Stopping ThreadPool");

  queue_.Close();

  for (auto& worker_thread : worker_threads_) {
    worker_thread.join();
  }

  LOG_DEBUG(fmt::format("Number of uncompleted tasks in Thread Pool is {}", queue_.Size()));
}

void ThreadPool::AddTask(Worker&& worker) {
  if (!is_started) {
    LOG_ERROR("ThreadPool can not be filled since it wasn't started\n");
    throw std::runtime_error("ThreadPool can not be filled since it wasn't started\n");
  }
  queue_.PushBack(std::move(worker));
}

ThreadPool::~ThreadPool() {
  if (!is_started) {
    LOG_ERROR("ThreadPool wasn't stopped manually\n");
    Stop();
  }
}