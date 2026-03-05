#include "fiber.hpp"
#include "logging/logging.hpp"
#include "scheduling/fiber/thread_pool/thread_pool.hpp"

namespace MyAsyncFramework::scheduling::fiber::core {

thread_local Fiber* current_fiber_thrloc;

void Fiber::operator()() {
  current_fiber_thrloc = this;
  coroutine_.Resume();
  if (!coroutine_.IsDone()) {
    thread_pool_.AddTask([this] {
      (*this)();
    });
  } else {
    delete this;
  }
}

Fiber& Fiber::Self() {
  return *current_fiber_thrloc;
}

thread_pool::ThreadPool& Fiber::GetThreadPool() {
  return thread_pool_;
}

void Fiber::Schedule() {
  thread_pool_.AddTask([this]{
    (*this)();
  });
}

void Fiber::Yield() {
  coroutine_.Suspend();
}

} // namespace MyAsyncFramework::scheduling::fiber::core