#include "debug-executor.hpp"

#include <thread>
#include <chrono>

#include <my-async-framework/logging/logging.hpp>

namespace MyServer::executors {

void DebugExecutor(Args&& args) {
  std::ignore = args;
  static std::atomic<int> total_called = 0;

  total_called.fetch_add(1);
  LOG_INFO(fmt::format("Total called: {}", total_called.load()));
  LOG_INFO(fmt::format("Handling connection number {}", total_called.load()));
  std::this_thread::sleep_for(std::chrono::seconds(3));
}

} // namespace MyServer::executors