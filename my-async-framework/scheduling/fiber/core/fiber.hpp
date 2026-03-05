#pragma once

#include <my-async-framework/scheduling/fiber/thread_pool/thread_pool.hpp>
#include <my-async-framework/scheduling/fiber/thread_pool/worker.hpp>

#include "body.hpp"
#include "coroutine.hpp"

namespace MyAsyncFramework::scheduling::fiber::core {

class Fiber { 
 public:
  template <typename F>
  explicit Fiber(thread_pool::ThreadPool& thread_pool, Args&& args, F&& body)
    : coroutine_(std::move(args), std::forward<F>(body)),
      thread_pool_(thread_pool) {}
  
  void Schedule();
  void Yield();
  static Fiber& Self();
  thread_pool::ThreadPool& GetThreadPool();
  
 private:
  void operator()();
 private:
  Body body_;
  Coroutine coroutine_;
  thread_pool::ThreadPool& thread_pool_;
};


  
} // namespace MyAsyncFramework::scheduling::fiber::core