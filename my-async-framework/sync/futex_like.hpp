#pragma once

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <my-async-framework/sync/atomic.hpp>

namespace MyAsyncFramework::sync {

// https://github.com/torvalds/linux/blob/439fc29dfd3b9c072dfff292d91cfa2f6cfb702b/kernel/futex/waitwake.c#L16
inline void futex_wait(volatile Atomic* futex_addr, int expected_val) {
  syscall(SYS_futex, futex_addr, FUTEX_WAIT, expected_val, NULL, NULL, 0);
}

inline void futex_wake(volatile Atomic* futex_addr, int count) {
  syscall(SYS_futex, futex_addr, FUTEX_WAKE, count, NULL, NULL, 0);
}

inline void futex_wake_all(volatile Atomic* futex_addr) {
  futex_wake(futex_addr, INT_MAX);
}

} // namespace MyAsyncFramework::sync