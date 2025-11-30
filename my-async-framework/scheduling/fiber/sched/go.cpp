#include "go.hpp"

#include <my-async-framework/scheduling/fiber/core/fiber.hpp>

namespace MyAsyncFramework::scheduling::fiber::sched {

using Fiber = scheduling::fiber::core::Fiber;

void Go(ThreadPool& thread_pool, Args&& args, Body&& body) {
  Fiber* fiber_ptr = new Fiber(thread_pool, std::move(args), std::move(body));
  fiber_ptr->Schedule();
}

void Go(ThreadPool& thread_pool, Args&& args, const Body& body) {
  Fiber* fiber_ptr = new Fiber(thread_pool, std::move(args), std::move(body));
  fiber_ptr->Schedule();
}

void Go(Args&& args, Body&& body) {
  ThreadPool& thread_pool = Fiber::Self().GetThreadPool(); // TODO: Examine carefully why not data race
  Go(thread_pool, std::move(args), std::move(body));
}
  
} // namespace MyAsyncFramework::scheduling::fiber::sched