/*
cd "/home/ubuntu/recap/lockfree/queues/tests/"

clang++ -fsanitize=leak -std=c++20 spsc-bounded-queue.cpp -o spsc-bounded-queue.trash
clang++ -fsanitize=leak,address -std=c++20 spsc-bounded-queue.cpp -o spsc-bounded-queue.trash

"/home/ubuntu/recap/lockfree/queues/tests/"spsc-bounded-queue.trash
*/
#include "../spsc-bounded-queue.hpp"

#include "common/spsc.hpp"

int main() {
  {
    SPSCBoundedQueue<int> queue(4);
    TestSingleThreadBasic(queue);
  }
  {
    SPSCBoundedQueue<int> queue(8);
    TestSingleThreadFullAndEmpty(queue);
  }
  {
    SPSCBoundedQueue<int> queue(8);
    TestSingleThreadWrapAround(queue);
  }
  {
    SPSCBoundedQueue<int> queue(16);
    TestMultiThreadSPSC_CorrectOrder(queue);
  }
  {
    SPSCBoundedQueue<int> queue(4096);
    TestMultiThreadSPSC_Stress(queue);
  }
}