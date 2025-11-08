// cmake .. && cmake --build . && ./my-async-framework/test-mutex

#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>

#include <gtest/gtest.h>

#include <my-async-framework/sync/mutex.hpp>
#include <my-async-framework/sync/condition_variable.hpp>

using namespace MyAsyncFramework::sync;

TEST(MutexTest, BasicMutex) {
  Mutex mutex;
  mutex.Lock();
  mutex.Unlock();
}

TEST(MutexTest, MutexCriticalSection) {
  Mutex mutex;
  int shared_int = 0;

  std::vector<std::thread> threads;

  for (int i = 0; i < 10; ++i) {
    threads.emplace_back([&shared_int, &mutex]{
      for (int i = 0; i < 100'000; ++i) {
        std::unique_lock lock(mutex);
        ++shared_int;
      }
    });
  }

  for (int i = 0; i < 10; ++i) {
    threads[i].join();
  }

  ASSERT_EQ(shared_int, 100'000 * 10);
}

template <typename Mutex>
long long CheckMutexSpeed(Mutex& mutex) {
  int shared_int = 0;
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 300'000; ++i) {
    std::unique_lock lock(mutex);
    ++shared_int;
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  return time;
}

TEST(MutexTest, MutexSpeed) {
  Mutex mutex;
  auto my_mutex_time = CheckMutexSpeed<Mutex>(mutex);

  std::mutex std_mutex;
  auto std_mutex_time = CheckMutexSpeed<std::mutex>(std_mutex);
  ASSERT_LT(my_mutex_time, std_mutex_time * 3);
}


TEST(Storage, LifetimeIssue) {
  class Event {
   public:
    void Wait() {
      std::unique_lock<Mutex> locker(mutex_);
      while (!ready_) {
        fire_.Wait(locker);
      }
    }
  
    void Fire() {
      std::lock_guard<Mutex> guard(mutex_);
      ready_ = true;
      fire_.NotifyOne();
    }
  
   private:
    bool ready_{false};
    Mutex mutex_;
    ConditionVariable fire_;
  };

  for (int i = 0; i < 100'000; ++i) {
    auto event = new Event{};
    // TODO: use determenistic testing for guarantee fail in case of bad realization
    std::thread t([event] {
      event->Fire();
    });
    
    event->Wait();
    delete event;
    t.join();
  }
}
