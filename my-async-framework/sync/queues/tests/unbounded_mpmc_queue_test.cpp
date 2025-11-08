// cmake .. && cmake --build . && ./my-async-framework/test-unbounded-mpmc-queue

#include <chrono>
#include <iostream>
#include <thread>

#include <gtest/gtest.h>

#include <my-async-framework/sync/mutex.hpp>
#include <my-async-framework/sync/condition_variable.hpp>
#include <my-async-framework/sync/queues/unbounded_mpmc_queue.hpp>

using namespace MyAsyncFramework::sync;
using namespace MyAsyncFramework::sync::queues;

TEST(UnboundedMpMcQueueTest, Basic) {
  UnboundedMpMcQueue<int> queue;
  for (int i = 0; i < 10; ++i) {
    queue.PushBack(i);
  }

  for (int i = 0; i < 10; ++i) {
    ASSERT_EQ(queue.PopFront().value(), i);
  }

  queue.Close();
  ASSERT_EQ(queue.PopFront(), std::nullopt);
}

TEST(UnboundedMpMcQueueTest, Locking) {
  UnboundedMpMcQueue<int> queue;
  bool visited = false;

  std::thread t1([&queue, &visited]{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(visited, false);
    queue.PushBack(5);
  });


  auto t = queue.PopFront();
  visited = true;
  ASSERT_EQ(t.value(), 5);

  t1.join();
}

struct Base {
  Base() { std::cout << "Base default constructor\n"; ++default_constr; }
  Base(Base&& other) { std::cout << "Base move constructor\n"; ++move_constr; }
  Base(const Base& other) { std::cout << "Base copy constructor\n"; ++copy_constr; }
  Base& operator=(const Base& other) { std::cout << "Base operator= copy\n"; ++operator_eq_copy; return *this; }
  Base& operator=(Base&& other) { std::cout << "Base operator= move\n"; ++operator_eq_move; return *this; }
  ~Base() { std::cout << "Base destructor\n"; ++destructor; }
  
  static int default_constr;
  static int move_constr;
  static int copy_constr;
  static int operator_eq_copy;
  static int operator_eq_move;
  static int destructor;
};

bool operator==(const Base& v1, const Base& v2) { std::cout << "Base operator==\n"; return true; }

int Base::default_constr = 0;
int Base::move_constr = 0;
int Base::copy_constr = 0;
int Base::operator_eq_copy = 0;
int Base::operator_eq_move = 0;
int Base::destructor = 0;

TEST(UnboundedMpMcQueueTest, NoCopies) {
  UnboundedMpMcQueue<Base> queue;
  
  queue.PushBack(Base());
  
  ASSERT_EQ(Base::default_constr, 1);
  ASSERT_EQ(Base::move_constr, 1);
  ASSERT_EQ(Base::copy_constr, 0);
  ASSERT_EQ(Base::operator_eq_copy, 0);
  ASSERT_EQ(Base::operator_eq_move, 0);
  ASSERT_EQ(Base::destructor, 1);

  auto t = queue.PopFront();
  ASSERT_EQ(Base::default_constr, 1);
  ASSERT_EQ(Base::move_constr, 3);
  ASSERT_EQ(Base::copy_constr, 0);
  ASSERT_EQ(Base::operator_eq_copy, 0);
  ASSERT_EQ(Base::operator_eq_move, 0);
  ASSERT_EQ(Base::destructor, 3);
}