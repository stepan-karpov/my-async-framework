#include "yield.hpp"

#include <my-async-framework/scheduling/fiber/core/fiber.hpp>

namespace MyAsyncFramework::scheduling::fiber::sched {

using Fiber = scheduling::fiber::core::Fiber;

void Yield() {
  Fiber::Self().Yield();
}
  
} // namespace MyAsyncFramework::scheduling::fiber::sched