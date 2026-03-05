#pragma once

#include <my-async-framework/scheduling/fiber/core/body.hpp>
#include <my-async-framework/scheduling/fiber/thread_pool/thread_pool.hpp>
#include <my-async-framework/scheduling/fiber/core/fiber.hpp>

namespace MyAsyncFramework::scheduling::fiber::sched {

namespace {

using Fiber = scheduling::fiber::core::Fiber;
using Body = MyAsyncFramework::scheduling::fiber::core::Body;
using Args = MyAsyncFramework::scheduling::fiber::core::Args;
using ThreadPool = MyAsyncFramework::scheduling::fiber::thread_pool::ThreadPool;

};

template <typename Body>
void Go(ThreadPool& thread_pool, Args&& args, Body&& body) {
  Fiber* fiber_ptr = new Fiber(thread_pool, std::move(args), std::forward<Body>(body));
  fiber_ptr->Schedule();
}

template <typename Body>
void Go(Args&& args, Body&& body) {
  Go(Fiber::Self().GetThreadPool(), std::move(args), std::forward<Body>(body));
}

} // namespace MyAsyncFramework::scheduling::fiber::sched