/*
cd "/home/ubuntu/recap/lockfree/queues/tests/"

clang++ -fsanitize=leak -std=c++20 mpsc-unbounded-queue.cpp -o mpsc-unbounded-queue.trash
clang++ -fsanitize=leak,address -std=c++20 mpsc-unbounded-queue.cpp -o mpsc-unbounded-queue.trash

"/home/ubuntu/recap/lockfree/queues/tests/"mpsc-unbounded-queue.trash
*/
#include "../mpsc-unbounded-queue.hpp"

#include "common/mpsc.hpp"

void TestQueue() {
  {
    MPSCUnboundedQueue<int> queue;
    queue::TestSingleThreadBasic(queue);
  }
  {
    MPSCUnboundedQueue<int> queue;
    queue::TestSingleThreadFullAndEmpty(queue);
  }
  {
    MPSCUnboundedQueue<int> queue;
    queue::TestSingleThreadWrapAround(queue);
  }
  {
    MPSCUnboundedQueue<int> queue;
    queue::TestMultiThreadMPSC_Stress(queue);
  }
}

void TestLazyQueue() {
  {
    MPSCUnboundedLazyQueue<int> queue;
    lazy_queue::TestSingleThreadBasic(queue);
  }
  {
    MPSCUnboundedLazyQueue<int> queue;
    lazy_queue::TestSingleThreadFullAndEmpty(queue);
  }
  {
    MPSCUnboundedLazyQueue<int> queue;
    lazy_queue::TestSingleThreadWrapAround(queue);
  }
  {
    MPSCUnboundedLazyQueue<int> queue;
    lazy_queue::TestMultiThreadMPSC_Stress(queue);
  }
}

int main() {
  TestQueue();
  TestLazyQueue();
}