// cmake .. && cmake --build . && ./my-async-framework/test-condition-variable

#include <thread>
#include <condition_variable>

#include <gtest/gtest.h>

#include <my-async-framework/sync/mutex.hpp>
#include <my-async-framework/sync/condition_variable.hpp>

using namespace MyAsyncFramework::sync;

TEST(ConditionVariableTest, Basic) {
  int sleeping = 0;
  bool visited = false;
  Mutex mutex;
  ConditionVariable cond_var;

  std::thread t1([&]{
    std::lock_guard<Mutex> lock(mutex);
    ASSERT_EQ(visited, false);
    while (!visited) {
      ++sleeping;
      cond_var.Wait(mutex);
    }
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  ASSERT_EQ(sleeping, 1);
  visited = true;
  cond_var.NotifyOne();

  t1.join();
}