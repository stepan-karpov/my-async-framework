#pragma once

#include <my-async-framework/sync/atomic.hpp>

namespace MyAsyncFramework::sync {

#ifdef __linux__

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

inline void futex_wait(volatile Atomic* futex_addr, int expected_val) {
  syscall(SYS_futex, futex_addr, FUTEX_WAIT, expected_val, NULL, NULL, 0);
}

inline void futex_wake(volatile Atomic* futex_addr, int count) {
  syscall(SYS_futex, futex_addr, FUTEX_WAKE, count, NULL, NULL, 0);
}

inline void futex_wake_all(volatile Atomic* futex_addr) {
  futex_wake(futex_addr, INT_MAX);
}

#else

#include <pthread.h>
#include <errno.h>

inline pthread_mutex_t g_futex_mutex = PTHREAD_MUTEX_INITIALIZER;
inline pthread_cond_t g_futex_cond = PTHREAD_COND_INITIALIZER;

// not a true futex!
inline void futex_wait(volatile Atomic *futex_addr, int expected_val) {
  pthread_mutex_lock(&g_futex_mutex);
  while (*futex_addr == expected_val) {
    pthread_cond_wait(&g_futex_cond, &g_futex_mutex);
  }
  pthread_mutex_unlock(&g_futex_mutex);
}

// not a true futex!
inline void futex_wake(volatile Atomic *futex_addr, int count) {
  (void) futex_addr;
  pthread_mutex_lock(&g_futex_mutex);
  /*
  this piece of shit doesn't work, spend all night debugging :(
  use broadcast instead for MacOs

  if (count == 1) {
    pthread_cond_signal(&g_futex_cond);
  } else {
    pthread_cond_broadcast(&g_futex_cond);
  }
  */
  pthread_cond_broadcast(&g_futex_cond);

  pthread_mutex_unlock(&g_futex_mutex);
}

// not a true futex!
inline void futex_wake_all(volatile Atomic *futex_addr) {
  (void) futex_addr;
  pthread_mutex_lock(&g_futex_mutex);
  pthread_cond_broadcast(&g_futex_cond);
  pthread_mutex_unlock(&g_futex_mutex);
}

#endif

} // namespace MyAsyncFramework::sync