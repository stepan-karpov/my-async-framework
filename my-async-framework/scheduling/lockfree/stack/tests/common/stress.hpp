#include <atomic>
#include <thread>
#include <vector>
#include <cassert>
#include <iostream>
#include <optional>

template <typename Stack>
void TestStressPushManyThreads(Stack& stack) {

  constexpr int kThreads = 8;
  constexpr int kPerThread = 8'000'000 / kThreads;
  constexpr int kTotal = kThreads * kPerThread;

  auto pusher = [&](int tid) {
    for (int i = 0; i < kPerThread; ++i) {
      stack.Push((tid << 24) ^ i);
    }
  };

  std::vector<std::thread> threads;
  threads.reserve(kThreads);
  for (int t = 0; t < kThreads; ++t) {
    threads.emplace_back(pusher, t);
  }
  for (auto& thread : threads) {
    thread.join();
  }

  int drained = 0;
  while (stack.Pop().has_value()) {
    ++drained;
  }

  assert(drained == kTotal);
  std::cout << "TestStressPushManyThreads pushes=" << drained
            << " expected=" << kTotal << "\n";
}

template <typename Stack>
void TestStressPopManyThreads(Stack& stack) {
  constexpr int kPopThreads = 8;
  constexpr int kPrefill = 8'000'000;

  for (int i = 0; i < kPrefill; ++i) stack.Push(i);

  std::atomic<long long> pops{0};

  auto popper = [&] {
    while (true) {
      auto v = stack.Pop();
      if (!v.has_value()) break;
      pops.fetch_add(1, std::memory_order_relaxed);
    }
  };

  std::vector<std::thread> threads;
  threads.reserve(kPopThreads);
  for (int t = 0; t < kPopThreads; ++t) {
    threads.emplace_back(popper);
  }
  for (auto& thread : threads) {
    thread.join();
  }

  assert(pops.load() == kPrefill);
  std::cout << "TestStressPopManyThreads pops=" << pops.load()
            << " expected=" << kPrefill << "\n";
}

template <typename Stack>
void TestStressPushPopTogether(Stack& stack) {
  constexpr int kPushThreads = 4;
  constexpr int kPopThreads = 4;
  constexpr int kOps = 20'000'000;

  std::atomic<int> pushes_done{0};
  std::atomic<int> pops_done{0};

  auto pusher = [&](int tid) {
    while (true) {
      int i = pushes_done.fetch_add(1, std::memory_order_relaxed);
      if (i >= kOps) break;
      stack.Push((tid << 24) ^ i);
    }
  };

  auto popper = [&] {
    while (true) {
      int already = pops_done.load(std::memory_order_relaxed);
      if (already >= kOps) break;

      auto v = stack.Pop();
      if (!v.has_value()) {
        // стек временно пуст: push'еры могли не успеть
        continue;
      }

      int after = pops_done.fetch_add(1, std::memory_order_relaxed) + 1;
      if (after >= kOps) break;
    }
  };

  std::vector<std::thread> threads;
  threads.reserve(kPushThreads + kPopThreads);

  for (int t = 0; t < kPushThreads; ++t) { threads.emplace_back(pusher, t); }
  for (int t = 0; t < kPopThreads; ++t) { threads.emplace_back(popper); }

  for (auto& thread : threads) { thread.join(); }

  assert(pushes_done.load() >= kOps);
  assert(pops_done.load() == kOps);
  assert(stack.Pop() == std::nullopt);
  std::cout << "TestStressPushPopTogether "
            << "pushes_done=" << pushes_done.load() << " "
            << "pops_done=" << pops_done.load()
            << "\n";
}