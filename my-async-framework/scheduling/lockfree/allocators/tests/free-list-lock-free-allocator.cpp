/*
cd "/home/ubuntu/recap/lockfree/allocators/tests/"

clang++ -fsanitize=leak -std=c++20 free-list-lock-free-allocator.cpp -o free-list-lock-free-allocator.trash
clang++ -fsanitize=leak,address -std=c++20 free-list-lock-free-allocator.cpp -o free-list-lock-free-allocator.trash

"/home/ubuntu/recap/lockfree/allocators/tests/"free-list-lock-free-allocator.trash
*/
#include "../free-list-lock-free-allocator.hpp"

#include "common/unit.hpp"

int main() {
  {
    FreeListLockfreeAllocator<int64_t> allocator(4);
    TestBasic(allocator);
  }
  {
    FreeListLockfreeAllocator<int64_t> allocator(4);
    TestMany(allocator);
  }
  {
    FreeListLockfreeAllocator<int64_t> allocator(4);
    TestOverflow(allocator);
  }
}