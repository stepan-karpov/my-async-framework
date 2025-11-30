#include <thread>

#include <gtest/gtest.h>

#include <my-async-framework/scheduling/fiber/core/coroutine.hpp>

using namespace MyAsyncFramework::scheduling::fiber::core;

TEST(CoroutineTest, Basic) {
  int counter = 0;

  Coroutine coro(Args{42}, [&counter](Args&& args) {
    ASSERT_EQ(args.descriptor_, 42);
    for (size_t i = 0; i < 10'000; ++i) {
      ++counter;
    }
  });

  coro.Resume();
  ASSERT_TRUE(coro.IsDone());
  ASSERT_EQ(counter, 10'000);
}
