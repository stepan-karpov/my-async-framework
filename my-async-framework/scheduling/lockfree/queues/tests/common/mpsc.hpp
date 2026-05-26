#pragma once

#include <thread>
#include <cassert>
#include <iostream>
#include <vector>

namespace queue {

template <typename Queue>
void TestSingleThreadBasic(Queue& queue) {
  queue.Push(1);
  queue.Push(2);
  queue.Push(3);
  queue.Push(4);

  int x = -1;
  assert(queue.Pop(x) == true && x == 1);
  assert(queue.Pop(x) == true && x == 2);
  assert(queue.Pop(x) == true && x == 3);
  assert(queue.Pop(x) == true && x == 4);
  assert(queue.Pop(x) == false);

  std::cout << "TestSingleThreadBasic passed\n";
}

template <typename Queue>
void TestSingleThreadFullAndEmpty(Queue& queue) {
  int res = -1;
  
  assert(queue.Pop(res) == false);

  for (int i = 0; i < 8; ++i) {
    queue.Push(i);
    assert(queue.Pop(res) == true && res == i);
  }
  assert(queue.Pop(res) == false);

  std::cout << "TestSingleThreadFullAndEmpty passed\n";
}

template <typename Queue>
void TestSingleThreadWrapAround(Queue& queue) {
  int res = -1;

  // первый круг
  for (int i = 0; i < 4; ++i) {
    queue.Push(i);
    assert(queue.Pop(res) == true && res == i);
  }

  // второй круг (проверка wrap-around по маске)
  for (int i = 10; i < 20; ++i) {
    queue.Push(i);
    assert(queue.Pop(res) == true && res == i);
  }

  assert(queue.Pop(res) == false);

  std::cout << "TestSingleThreadWrapAround passed\n";
}

template <typename Queue>
void TestMultiThreadMPSC_Stress(Queue& queue) {
  constexpr int kProducers = 4;
  constexpr int kPerProducer = 250'000;
  constexpr int kTotal = kProducers * kPerProducer;

  std::atomic<int> producers_done{0};

  auto producer = [&](int tid) {
    const int base = tid * kPerProducer;
    for (int i = 0; i < kPerProducer; ++i) {
      queue.Push(base + i);
    }
    producers_done.fetch_add(1, std::memory_order_relaxed);
  };

  std::vector<std::thread> producers;
  producers.reserve(kProducers);
  for (int t = 0; t < kProducers; ++t) {
    producers.emplace_back(producer, t);
  }

  // single consumer
  std::vector<uint8_t> seen(static_cast<size_t>(kTotal), 0);
  int received = 0;

  while (received < kTotal) {
    int x = 0;
    if (!queue.Pop(x)) {
      if (producers_done.load(std::memory_order_relaxed) == kProducers) {
        break; // producers finished and queue empty
      }
      std::this_thread::yield();
      continue;
    }

    assert(0 <= x && x < kTotal);
    assert(seen[static_cast<size_t>(x)] == 0);
    seen[static_cast<size_t>(x)] = 1;
    ++received;
  }

  for (auto& th : producers) th.join();

  assert(received == kTotal);

  // очередь должна быть пуста
  int tmp = 0;
  assert(!queue.Pop(tmp));

  std::cout << "TestMultiThreadMPSC_Stress passed\n";
}
  
} // namespace queue


namespace lazy_queue {

template <typename Queue>
void TestSingleThreadBasic(Queue& queue) {
  queue.Push(1);
  queue.Push(2);
  queue.Push(3);
  queue.Push(4);

  assert(queue.Pop() == std::vector({4, 3, 2, 1}));
  assert(queue.Pop().empty());

  std::cout << "TestSingleThreadBasic passed\n";
}

template <typename Queue>
void TestSingleThreadFullAndEmpty(Queue& queue) {
  assert(queue.Pop().empty());

  for (int i = 0; i < 8; ++i) {
    queue.Push(i);
    assert(queue.Pop() == std::vector({i}));
  }
  assert(queue.Pop().empty());

  std::cout << "TestSingleThreadFullAndEmpty passed\n";
}

template <typename Queue>
void TestSingleThreadWrapAround(Queue& queue) {
  // первый круг
  for (int i = 0; i < 4; ++i) {
    queue.Push(i);
    assert(queue.Pop() == std::vector({i}));
  }

  // второй круг (проверка wrap-around по маске)
  for (int i = 10; i < 20; ++i) {
    queue.Push(i);
    assert(queue.Pop() == std::vector({i}));
  }

  assert(queue.Pop().empty());

  std::cout << "TestSingleThreadWrapAround passed\n";
}

template <typename Queue>
void TestMultiThreadMPSC_Stress(Queue& queue) {
  constexpr int kProducers = 4;
  constexpr int kPerProducer = 250'000;
  constexpr int kTotal = kProducers * kPerProducer;

  std::atomic<int> producers_done{0};

  auto producer = [&](int tid) {
    const int base = tid * kPerProducer;
    for (int i = 0; i < kPerProducer; ++i) {
      queue.Push(base + i);
    }
    producers_done.fetch_add(1, std::memory_order_relaxed);
  };

  std::vector<std::thread> producers;
  producers.reserve(kProducers);
  for (int t = 0; t < kProducers; ++t) {
    producers.emplace_back(producer, t);
  }

  // single consumer
  std::vector<uint8_t> seen(static_cast<size_t>(kTotal), 0);
  int received = 0;

  while (received < kTotal) {
    auto batch = queue.Pop();                 // std::vector<int>
    if (batch.empty()) {
      if (producers_done.load(std::memory_order_relaxed) == kProducers) {
        // producers закончились, а очередь пуста => если что-то потеряли, выйдем с assert ниже
        break;
      }
      std::this_thread::yield();
      continue;
    }

    for (int x : batch) {
      assert(0 <= x && x < kTotal);
      assert(seen[static_cast<size_t>(x)] == 0);  // no duplicates
      seen[static_cast<size_t>(x)] = 1;
      ++received;
    }
  }

  for (auto& th : producers) th.join();

  assert(received == kTotal);
  // опционально: очередь должна быть пуста
  assert(queue.Pop().empty());

  std::cout << "TestMultiThreadMPSC_Stress passed\n";
}
  
} // namespace lazy_queue
