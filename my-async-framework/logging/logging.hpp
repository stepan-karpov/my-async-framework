#pragma once

#include <iostream>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <fmt/format.h>

// Thread-safe logging mutex (one instance shared across all translation units)
inline std::mutex log_mutex;

// configure in static_settings.hpp
#define DEFINED_LOG_LEVEL 0

#ifdef LOG_LEVEL_DEBUG
#define DEFINED_LOG_LEVEL 0
#elif defined(LOG_LEVEL_WARNING)
#define DEFINED_LOG_LEVEL 1
#elif defined(LOG_LEVEL_INFO)
#define DEFINED_LOG_LEVEL 2
#elif defined(LOG_LEVEL_ERROR)
#define DEFINED_LOG_LEVEL 3
#endif

#define LOG_IMPL(level, msg, level_value) do { \
  if (level_value < DEFINED_LOG_LEVEL) { \
    break; \
  } \
  std::lock_guard<std::mutex> lock(log_mutex); \
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