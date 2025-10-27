#include <iostream>
#include <chrono>
#include <iomanip>

#define LOG_INFO(msg) do { \
  auto now = std::chrono::system_clock::now(); \
  auto time_point = std::chrono::system_clock::to_time_t(now); \
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000; \
  std::tm* local_time = std::localtime(&time_point); \
  std::cout << "[" << std::put_time(local_time, "%H:%M:%S") << "." \
            << std::setfill('0') << std::setw(3) << ms.count() << "] " \
            << "[INFO] " << msg << std::endl; \
} while(0)

#define LOG_DEBUG(msg) do { \
  auto now = std::chrono::system_clock::now(); \
  auto time_point = std::chrono::system_clock::to_time_t(now); \
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000; \
  std::tm* local_time = std::localtime(&time_point); \
  std::cout << "[" << std::put_time(local_time, "%H:%M:%S") << "." \
            << std::setfill('0') << std::setw(3) << ms.count() << "] " \
            << "[DEBUG] " << msg << std::endl; \
} while(0)

#define LOG_ERROR(msg) do { \
  auto now = std::chrono::system_clock::now(); \
  auto time_point = std::chrono::system_clock::to_time_t(now); \
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000; \
  std::tm* local_time = std::localtime(&time_point); \
  std::cerr << "[" << std::put_time(local_time, "%H:%M:%S") << "." \
            << std::setfill('0') << std::setw(3) << ms.count() << "] " \
            << "[ERROR] " << msg << std::endl; \
} while(0)

int main() {
  LOG_INFO("Hello, world!");
  LOG_DEBUG("This is a debug message");
  LOG_ERROR("This is an error message");
  
  return 0;
}