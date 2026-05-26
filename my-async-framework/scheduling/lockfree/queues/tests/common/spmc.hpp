#pragma once

#include <optional>
#include <thread>
#include <cassert>
#include <iostream>
#include <vector>

template <typename Queue>
void TestSingleThreadBasic(Queue& queue) {
  assert(queue.Push(1));
  assert(queue.Push(2));
  assert(queue.Push(3));
  assert(queue.Push(4));
  assert(!queue.Push(5));               // full

  assert(queue.Pop() == std::optional{1});
  assert(queue.Pop() == std::optional{2});
  assert(queue.Pop() == std::optional{3});
  assert(queue.Pop() == std::optional{4});
  assert(queue.Pop() == std::nullopt);  // empty

  std::cout << "TestSingleThreadBasic passed\n";
}

template <typename Queue>
void TestSingleThreadFullAndEmpty(Queue& queue) {
  assert(queue.Pop() == std::nullopt);

  // fill
  for (int i = 0; i < 8; ++i) {
    assert(queue.Push(i));
  }
  assert(!queue.Push(99));  // full

  // pop all
  for (int i = 0; i < 8; ++i) {
    assert(queue.Pop() == std::optional{i});
  }
  assert(queue.Pop() == std::nullopt);

  std::cout << "TestSingleThreadFullAndEmpty passed\n";
}

template <typename Queue>
void TestSingleThreadWrapAround(Queue& queue) {
  // первый круг
  for (int i = 0; i < 4; ++i) {
    assert(queue.Push(i));
    assert(queue.Pop() == std::optional{i});
  }

  // второй круг (проверка wrap-around по маске)
  for (int i = 10; i < 20; ++i) {
    assert(queue.Push(i));
    assert(queue.Pop() == std::optional{i});
  }

  assert(queue.Pop() == std::nullopt);

  std::cout << "TestSingleThreadWrapAround passed\n";
}

template <typename Queue>
void TestMultiThreadSPSC_CorrectOrder(Queue& queue) {
  constexpr int COUNT = 1000;

  std::thread producer([&]() {
    for (int i = 0; i < COUNT; ++i) {
      while (!queue.Push(i)) {
        std::this_thread::yield();  // ждём, пока не освободится место
      }
    }
  });

  std::thread consumer([&]() {
    int expected = 0;
    while (expected < COUNT) {
      auto val = queue.Pop();
      if (val) {
        assert(*val == expected);
        ++expected;
      } else {
        std::this_thread::yield();
      }
    }
  });

  producer.join();
  consumer.join();

  std::cout << "TestMultiThreadSPSC_CorrectOrder passed\n";
}

template <typename Queue>
void TestMultiThreadSPSC_Stress(Queue& queue) {
  constexpr uint64_t COUNT = 1'000'000;
  

  std::atomic<bool> producer_done{false};

  std::thread producer([&]() {
    for (uint64_t i = 0; i < COUNT; ++i) {
      while (!queue.Push(i)) {
        std::this_thread::yield();
      }
    }
    producer_done = true;
  });

  std::thread consumer([&]() {
    uint64_t expected = 0;
    while (true) {
      auto val = queue.Pop();
      if (val) {
        assert(*val == expected);
        ++expected;
        if (expected == COUNT) break;
      } else if (producer_done.load(std::memory_order_relaxed)) {
        // producer закончил, а мы всё ещё читаем — проверяем, что ничего не потеряли
        assert(expected == COUNT);
        break;
      } else {
        std::this_thread::yield();
      }
    }
  });

  producer.join();
  consumer.join();

  std::cout << "TestMultiThreadSPSC_Stress passed\n";
}

template <typename Queue>
void TestMultiThreadSPMC_ManyConsumers(Queue& queue) {
  constexpr int COUNT = 1'000'000;
  constexpr int CONSUMERS = 4;

  std::atomic<bool> producer_done{false};

  // Для проверки уникальности: значения 0..COUNT-1 должны встретиться ровно по разу
  std::vector<std::atomic<uint8_t>> seen(COUNT);
  for (auto& x : seen) x.store(0, std::memory_order_relaxed);

  std::atomic<int> received{0};

  std::thread producer([&]() {
    for (int i = 0; i < COUNT; ++i) {
      while (!queue.Push(i)) {
        std::this_thread::yield();
      }
    }
    producer_done.store(true, std::memory_order_release);
  });

  auto consumer_fn = [&]() {
    while (true) {
      auto val = queue.Pop();
      if (val) {
        const int x = *val;
        assert(0 <= x && x < COUNT);

        // Если очередь корректна, каждый x должен встретиться ровно 1 раз
        uint8_t expected = 0;
        const bool ok = seen[static_cast<size_t>(x)].compare_exchange_strong(
            expected, 1, std::memory_order_relaxed);
        assert(ok);

        received.fetch_add(1, std::memory_order_relaxed);
      } else if (producer_done.load(std::memory_order_acquire)) {
        // Производитель закончил, очередь пуста
        break;
      } else {
        std::this_thread::yield();
      }
    }
  };

  std::vector<std::thread> consumers;
  consumers.reserve(CONSUMERS);
  for (int i = 0; i < CONSUMERS; ++i) {
    consumers.emplace_back(consumer_fn);
  }

  producer.join();
  for (auto& t : consumers) t.join();

  assert(received.load(std::memory_order_relaxed) == COUNT);
  for (int i = 0; i < COUNT; ++i) {
    assert(seen[static_cast<size_t>(i)].load(std::memory_order_relaxed) == 1);
  }

  std::cout << "TestMultiThreadSPMC_ManyConsumers passed\n";
}