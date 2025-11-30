#include <chrono>
#include <thread>

#include <gtest/gtest.h>


#include <my-async-framework/scheduling/fiber/thread_pool/thread_pool.hpp>
#include <my-async-framework/scheduling/fiber/sched/go.hpp>
#include <my-async-framework/scheduling/fiber/sched/yield.hpp>
#include <my-async-framework/sync/wait_group.hpp>

using namespace MyAsyncFramework::scheduling::fiber;
using namespace MyAsyncFramework::scheduling::fiber::core;
using namespace MyAsyncFramework::sync;

TEST(FiberTest, Basic) {
  thread_pool::ThreadPool thread_pool(4);
  thread_pool.Start();

  WaitGroup wg;
  int counter = 0;
  
  wg.Add(1);
  sched::Go(thread_pool, Args{42}, [&counter, &wg](Args&& args) {
    ASSERT_EQ(args.descriptor_, 42);
    for (size_t i = 0; i < 10'000; ++i) {
      counter++;
    }
    wg.Done();
  });

  wg.Wait();
  thread_pool.Stop();

  ASSERT_EQ(counter, 10'000);
}

TEST(FiberTest, BasicInner) {
  thread_pool::ThreadPool thread_pool(4);
  thread_pool.Start();

  WaitGroup wg;
  int counter = 0;

  wg.Add(1);
  sched::Go(thread_pool, Args{42}, [&counter, &wg](Args&& args) {
    ASSERT_EQ(args.descriptor_, 42);
    for (size_t i = 0; i < 10'000; ++i) {
      counter++;
    }
    wg.Add(1);
    sched::Go(Args{43}, [&counter, &wg](Args&& args) {
      ASSERT_EQ(args.descriptor_, 43);
      for (size_t i = 0; i < 10'000; ++i) {
        counter++;
      }
      wg.Done();
    });
    wg.Done();
  });

  wg.Wait();
  thread_pool.Stop();

  ASSERT_EQ(counter, 20'000);
}


TEST(FiberTest, GoGroup) {
  thread_pool::ThreadPool thread_pool(4);
  thread_pool.Start();

  WaitGroup wg;
  const size_t kFibers = 7;
  int counter = 0;

  for (size_t i = 0; i < kFibers; ++i) {
    wg.Add(1);
    sched::Go(thread_pool, Args{static_cast<int>(i)}, [&counter, &wg](Args&& args) {
      counter++;
      wg.Done();
    });
  }

  wg.Wait();
  thread_pool.Stop();

  ASSERT_EQ(counter, kFibers);
}

TEST(FiberTest, GoChild) {
  thread_pool::ThreadPool thread_pool(3);
  thread_pool.Start();

  WaitGroup wg;
  int counter = 0;

  wg.Add(1);
  sched::Go(thread_pool, Args{1}, [&counter, &wg](Args&& args) {
    ASSERT_EQ(args.descriptor_, 1);
    counter++;

    wg.Add(1);
    sched::Go(Args{2}, [&counter, &wg](Args&& args) {
      ASSERT_EQ(args.descriptor_, 2);
      counter++;
      wg.Done();
    });
    wg.Done();
  });

  wg.Wait();
  thread_pool.Stop();

  ASSERT_EQ(counter, 2);
}

TEST(FiberTest, Parallel) {
  const size_t kThreads = 4;

  thread_pool::ThreadPool thread_pool(kThreads);
  thread_pool.Start();

  WaitGroup wg;
  auto start = std::chrono::steady_clock::now();

  int counter = 0;
  for (size_t i = 0; i < kThreads; ++i) {
    wg.Add(1);
    sched::Go(thread_pool, Args{static_cast<int>(i)}, [&counter, &wg](Args&& args) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      counter++;
      wg.Done();
    });
  }

  wg.Wait();
  thread_pool.Stop();

  auto end = std::chrono::steady_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);

  ASSERT_EQ(counter, kThreads);
  ASSERT_TRUE(elapsed < std::chrono::seconds(2));
}

TEST(FiberTest, Yield) {
  thread_pool::ThreadPool thread_pool(1);
  thread_pool.Start();

  WaitGroup wg;
  bool yielded = false;
  
  wg.Add(1);
  sched::Go(thread_pool, Args{1}, [&yielded, &wg](Args&& args) {
    sched::Yield();
    yielded = true;
    wg.Done();
  });

  wg.Wait();
  thread_pool.Stop();

  ASSERT_TRUE(yielded);
}

TEST(FiberTest, YieldChild) {
  thread_pool::ThreadPool thread_pool(1);
  thread_pool.Start();

  WaitGroup wg;
  bool child = false;

  wg.Add(1);
  sched::Go(thread_pool, Args{1}, [&child, &wg](Args&& args) {
    wg.Add(1);
    sched::Go(Args{2}, [&child, &wg](Args&& args) {
      child = true;
      wg.Done();
    });

    while (!child) {
      sched::Yield();
    }
    wg.Done();
  });

  wg.Wait();
  thread_pool.Stop();

  ASSERT_TRUE(child);
}

TEST(FiberTest, ForYield) {
  thread_pool::ThreadPool thread_pool(1);
  thread_pool.Start();

  WaitGroup wg;
  const size_t kYields = 128;
  size_t yields = 0;

  wg.Add(1);
  sched::Go(thread_pool, Args{1}, [&yields, kYields, &wg](Args&& args) {
    for (size_t i = 0; i < kYields; ++i) {
      sched::Yield();
      ++yields;
    }
    wg.Done();
  });

  wg.Wait();
  thread_pool.Stop();

  ASSERT_EQ(yields, kYields);
}

TEST(FiberTest, PingPong) {
  thread_pool::ThreadPool thread_pool(1);
  thread_pool.Start();

  WaitGroup wg;
  bool start = false;
  int turn = 0;
  const size_t kRounds = 3;

  wg.Add(2);
  sched::Go(thread_pool, Args{1}, [&start, &turn, kRounds, &wg](Args&& args) {
    while (!start) {
      sched::Yield();
    }

    for (size_t i = 0; i < kRounds; ++i) {
      ASSERT_TRUE(turn == 0);
      turn ^= 1;
      sched::Yield();
    }
    wg.Done();
  });

  sched::Go(thread_pool, Args{2}, [&start, &turn, kRounds, &wg](Args&& args) {
    start = true;
    sched::Yield();

    for (size_t j = 0; j < kRounds; ++j) {
      ASSERT_TRUE(turn == 1);
      turn ^= 1;
      sched::Yield();
    }
    wg.Done();
  });

  wg.Wait();
  thread_pool.Stop();
}

TEST(FiberTest, YieldGroup) {
  thread_pool::ThreadPool thread_pool(4);
  thread_pool.Start();

  WaitGroup wg;
  const size_t kFibers = 5;
  const size_t kYields = 7;
  int counter = 0;

  for (size_t i = 0; i < kFibers; ++i) {
    wg.Add(1);
    sched::Go(thread_pool, Args{static_cast<int>(i)}, [&counter, kYields, &wg](Args&& args) {
      for (size_t j = 0; j < kYields; ++j) {
        sched::Yield();
      }
      counter++;
      wg.Done();
    });
  }

  wg.Wait();
  thread_pool.Stop();

  ASSERT_EQ(counter, kFibers);
}

TEST(FiberTest, TwoPools) {
  thread_pool::ThreadPool thread_pool1(3);
  thread_pool1.Start();

  thread_pool::ThreadPool thread_pool2(3);
  thread_pool2.Start();

  WaitGroup wg;
  int counter1 = 0;
  int counter2 = 0;

  wg.Add(2);
  sched::Go(thread_pool1, Args{1}, [&counter1, &wg](Args&& args) {
    for (size_t i = 0; i < 2; ++i) {
      sched::Yield();
    }
    counter1++;
    wg.Done();
  });

  sched::Go(thread_pool2, Args{2}, [&counter2, &wg](Args&& args) {
    for (size_t j = 0; j < 3; ++j) {
      sched::Yield();
    }
    counter2++;
    wg.Done();
  });

  wg.Wait();
  thread_pool1.Stop();
  thread_pool2.Stop();

  ASSERT_EQ(counter1, 1);
  ASSERT_EQ(counter2, 1);
}
