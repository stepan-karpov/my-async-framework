/*
cd "/home/ubuntu/recap/lockfree/queues/tests/"

clang++ -fsanitize=leak -std=c++20 spsc-bounded-queue.cpp -o spsc-bounded-queue.trash
clang++ -fsanitize=leak,address -std=c++20 spsc-bounded-queue.cpp -o spsc-bounded-queue.trash

"/home/ubuntu/recap/lockfree/queues/tests/"spsc-bounded-queue.trash
*/
#include "../spmc-bounded-queue.hpp"

#include "common/spmc.hpp"

int main() {
  {
    SPMCBoundedQueue<int> queue(4);
    TestSingleThreadBasic(queue);
  }
  {
    SPMCBoundedQueue<int> queue(8);
    TestSingleThreadFullAndEmpty(queue);
  }
  {
    SPMCBoundedQueue<int> queue(8);
    TestSingleThreadWrapAround(queue);
  }
  {
    SPMCBoundedQueue<int> queue(16);
    TestMultiThreadSPSC_CorrectOrder(queue);
  }
  {
    SPMCBoundedQueue<int> queue(4096);
    TestMultiThreadSPSC_Stress(queue);
  }
  {
    SPMCBoundedQueue<int> queue(4096);
    TestMultiThreadSPMC_ManyConsumers(queue);
  }
}