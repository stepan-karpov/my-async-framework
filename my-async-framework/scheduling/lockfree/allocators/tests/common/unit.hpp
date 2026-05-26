#pragma once

#include <iostream>

template <typename Allocator>
void TestBasic(Allocator& alloc) {
  using Traits = std::allocator_traits<Allocator>;
  using T = typename Traits::value_type;

  T* p = Traits::allocate(alloc, 1);
  assert(p != nullptr);

  Traits::construct(alloc, p, T{42});
  assert(*p == T{42});

  Traits::destroy(alloc, p);
  Traits::deallocate(alloc, p, 1);
  
  std::cout << "TestBasic passed\n";
}

template <typename Allocator>
void TestMany(Allocator& alloc) {
  using Traits = std::allocator_traits<Allocator>;
  using T = typename Traits::value_type;

  constexpr int n = 100;
  for (int i = 0; i < n; ++i) {
    T* p = Traits::allocate(alloc, 1);
    assert(p != nullptr);
    Traits::construct(alloc, p, T{static_cast<int>(i)});
    Traits::destroy(alloc, p);
    Traits::deallocate(alloc, p, 1);
  }

  std::cout << "TestMany passed\n";
}

template <typename Allocator>
void TestOverflow(Allocator& alloc) {
  using Traits = std::allocator_traits<Allocator>;
  using T = typename Traits::value_type;

  T* p1 = Traits::allocate(alloc, 1);
  T* p2 = Traits::allocate(alloc, 1);
  T* p3 = Traits::allocate(alloc, 1);
  T* p4 = Traits::allocate(alloc, 1);
  T* p5 = Traits::allocate(alloc, 1);

  assert(p1 && p2 && p3 && p4);
  assert(p5 == nullptr); // переполнились (capacity=4)

  // При желании проверяем, что адреса разные
  assert(p1 != p2 && p1 != p3 && p1 != p4);
  assert(p2 != p3 && p2 != p4);
  assert(p3 != p4);

  // Освобождаем и убеждаемся, что снова можно получить слот
  Traits::deallocate(alloc, p4, 1);
  T* p6 = Traits::allocate(alloc, 1);
  assert(p6 != nullptr);

  // cleanup (p6 это один из слотов, можно просто вернуть его тоже)
  Traits::deallocate(alloc, p6, 1);
  Traits::deallocate(alloc, p3, 1);
  Traits::deallocate(alloc, p2, 1);
  Traits::deallocate(alloc, p1, 1);

  std::cout << "TestOverflow passed\n";
}