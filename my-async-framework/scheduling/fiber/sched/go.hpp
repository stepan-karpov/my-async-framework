#pragma once

#include <my-async-framework/scheduling/fiber/core/body.hpp>
#include <my-async-framework/scheduling/fiber/thread_pool/thread_pool.hpp>

namespace MyAsyncFramework::scheduling::fiber::sched {

using Body = MyAsyncFramework::scheduling::fiber::core::Body;
using Args = MyAsyncFramework::scheduling::fiber::core::Args;
using ThreadPool = MyAsyncFramework::scheduling::fiber::thread_pool::ThreadPool;

void Go(ThreadPool&, Args&&, Body&&);

void Go(ThreadPool&, Args&&, const Body&);

void Go(Args&&, Body&&);

} // namespace MyAsyncFramework::scheduling::fiber::sched