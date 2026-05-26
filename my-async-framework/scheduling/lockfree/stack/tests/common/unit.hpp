#pragma once

#include <optional>
#include <iostream>

template <typename Stack>
void TestBasic(Stack& stack) {
  stack.Push(1);
  stack.Push(2);
  stack.Push(3);
  stack.Push(4);
  assert(stack.Pop().value() == 4);
  assert(stack.Pop().value() == 3);
  assert(stack.Pop().value() == 2);
  assert(stack.Pop().value() == 1);
  assert(stack.Pop() == std::nullopt);

  std::cout << "TestBasic passed\n";
}

template <typename Stack>
void TestBasicBounded(Stack& stack) {
  assert(stack.Push(1));
  assert(stack.Push(2));
  assert(stack.Push(3));
  assert(stack.Push(4));
  assert(!stack.Push(5));
  assert(stack.Pop().value() == 4);
  assert(stack.Pop().value() == 3);
  assert(stack.Pop().value() == 2);
  assert(stack.Pop().value() == 1);
  assert(stack.Pop() == std::nullopt);

  std::cout << "TestBasicBounded passed\n";
}