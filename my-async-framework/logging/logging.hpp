#pragma once

#include <iostream>
#include <chrono>
#include <iomanip>
#include <fmt/format.h>

#include <my-async-framework/sync/mutex.hpp>

// Thread-safe logging mutex (one instance shared across all translation units)
inline MyAsyncFramework::sync::Mutex log_mutex;

// Configure via CMake: -DLOG_LEVEL=0|1|2|3
// LOG_LEVEL numeric values: 0=DEBUG, 1=WARNING, 2=INFO, 3=ERROR
#ifndef DEFINED_LOG_LEVEL
  #ifdef LOG_LEVEL
    #define DEFINED_LOG_LEVEL LOG_LEVEL
  #else
    // Default to DEBUG if nothing is defined
    #define DEFINED_LOG_LEVEL 0
  #endif
#endif

#define LOG_IMPL(level, msg, level_value) do { \
  if (level_value < DEFINED_LOG_LEVEL) { \
    break; \
  } \
  std::lock_guard<MyAsyncFramework::sync::Mutex> lock(log_mutex); \
  auto now = std::chrono::system_clock::now(); \
  auto time_point = std::chrono::system_clock::to_time_t(now); \
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000; \
  std::tm* local_time = std::localtime(&time_point); \
  std::cout << "[" << std::put_time(local_time, "%H:%M:%S") << "." \
         << std::setfill('0') << std::setw(3) << ms.count() << "] " \
         << level << msg << std::endl; \
} while(0)

#define LOG_DEBUG(msg) LOG_IMPL("[DEBUG] ", msg, 0)
#define LOG_WARNING(msg) LOG_IMPL("[WARNING] ", msg, 1)
#define LOG_INFO(msg) LOG_IMPL("[INFO] ", msg, 2)
#define LOG_ERROR(msg) LOG_IMPL("[ERROR] ", msg, 3)