#include "fiber.hpp"
#include "logging/logging.hpp"
#include "scheduling/fiber/thread_pool/thread_pool.hpp"

namespace MyAsyncFramework::scheduling::fiber::core {

thread_local Fiber* current_fiber_thrloc;

FiberWrapper::FiberWrapper(Fiber* fiber)
  : fiber_(fiber) {}
  
FiberWrapper::FiberWrapper(const FiberWrapper& other)
  : fiber_(other.fiber_) {}

void FiberWrapper::operator()() {
  current_fiber_thrloc = fiber_;
  fiber_->coroutine_.Resume();
  if (!fiber_->coroutine_.IsDone()) {
    fiber_->thread_pool_.AddTask(FiberWrapper(fiber_));
  } else {
    delete fiber_;
  }
}

Fiber& Fiber::Self() {
  return *current_fiber_thrloc;
}

thread_pool::ThreadPool& Fiber::GetThreadPool() {
  return thread_pool_;
}

void Fiber::Schedule() {
  thread_pool_.AddTask(FiberWrapper(this));
}

void Fiber::Yield() {
  coroutine_.Suspend();
}

} // namespace MyAsyncFramework::scheduling::fiber::core