#include <thread>
#include <condition_variable>

#include <gtest/gtest.h>

#include <my-async-framework/sync/mutex.hpp>
#include <my-async-framework/sync/condition_variable.hpp>
#include <my-async-framework/scheduling/thread_pool.hpp>

using namespace MyAsyncFramework::sync;
using namespace MyAsyncFramework::scheduling;

TEST(ThreadPoolTest, Basic) {
  const int N = 100'000;
  ThreadPool pool(4);
  pool.Start();

  std::atomic<int> counter{0};
  for (int i = 0; i < N; ++i) {
    pool.AddTask(Worker([&counter](const int _) {
      counter.fetch_add(1);
    }, -1));
  }

  pool.Stop();
  ASSERT_EQ(counter.load(), N);
}

// TODO: add tests (unit + stress + intrusive)