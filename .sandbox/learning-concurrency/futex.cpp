#include <iostream>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <atomic>

using Atomic = typename std::atomic<int64_t>;
volatile Atomic g_futex_val = 0;

#ifdef __linux__

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

void futex_wait(volatile Atomic* futex_addr, int expected_val) {
    std::cout << "You're using Linux\n";
    syscall(SYS_futex, futex_addr, FUTEX_WAIT, expected_val, NULL, NULL, 0);
}

void futex_wake(volatile Atomic* futex_addr, int count) {
    std::cout << "You're using Linux\n";
    syscall(SYS_futex, futex_addr, FUTEX_WAKE, count, NULL, NULL, 0);
}

#else

#include <pthread.h>
#include <errno.h>

static pthread_mutex_t g_futex_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_futex_cond = PTHREAD_COND_INITIALIZER;

// not true futex!
void futex_wait(volatile Atomic *futex_addr, int expected_val) {
    std::cout << "You're using MacOs\n";
    pthread_mutex_lock(&g_futex_mutex);
    while (*futex_addr == expected_val) {
        pthread_cond_wait(&g_futex_cond, &g_futex_mutex);
    }
    pthread_mutex_unlock(&g_futex_mutex);
}

// not true futex!
void futex_wake(volatile Atomic *futex_addr, int count) {
    std::cout << "You're using MacOs\n";
    (void)futex_addr;
    pthread_mutex_lock(&g_futex_mutex);
    if (count == 1)
        pthread_cond_signal(&g_futex_cond);
    else
        pthread_cond_broadcast(&g_futex_cond);
    pthread_mutex_unlock(&g_futex_mutex);
}

#endif

void foo() {
  std::cout << "[Foo] foo start" << std::endl;
  futex_wait(&g_futex_val, 0);
  std::cout << "[Foo] Foo continue with g_futex_val = " << g_futex_val << std::endl;
}

void bar() {
  std::cout << "[Bar] bar start" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(3));

  std::cout << "[Bar] start waking Foo" << std::endl;
  g_futex_val = 1;
  futex_wake(&g_futex_val, 1);
}

int main() {
  std::thread t1(foo);
  std::thread t2(bar);

  t1.join();
  t2.join();

  return 0;
}
