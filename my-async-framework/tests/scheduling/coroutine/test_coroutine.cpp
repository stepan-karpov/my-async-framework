#include <thread>

#include <gtest/gtest.h>

#include <my-async-framework/scheduling/coroutine/coroutine.hpp>

using namespace MyAsyncFramework::scheduling::coroutine;

TEST(CoroutineTest, Basic) {
  Coroutine coro([](auto self) {
    self.Suspend();
  });

  coro.Resume();
  ASSERT_TRUE(!coro.IsDone());

  coro.Resume();
  ASSERT_TRUE(coro.IsDone());
}

TEST(CoroutineTest, StepByStep) {
  int step = 0;

  Coroutine a([&step](auto self) {
    step = 1;
    self.Suspend();
    step = 3;
  });

  Coroutine b([&step](auto self) {
    step = 2;
    self.Suspend();
    step = 4;
  });

  ASSERT_EQ(step, 0);
  a.Resume();
  ASSERT_EQ(step, 1);
  b.Resume();
  ASSERT_EQ(step, 2);

  a.Resume();
  ASSERT_EQ(step, 3);
  b.Resume();
  ASSERT_EQ(step, 4);

  ASSERT_TRUE(a.IsDone());
  ASSERT_TRUE(b.IsDone());
}
