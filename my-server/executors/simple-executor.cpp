#include "simple-executor.hpp"

#include <thread>
#include <chrono>

#include <my-async-framework/logging/logging.hpp>

void SimpleExecutor(const int socket) {
  static std::atomic<int> total_called = 0;
  LOG_INFO(fmt::format("Handling connection {}", socket));
  std::this_thread::sleep_for(std::chrono::seconds(10));
  total_called.fetch_add(1);

  LOG_INFO(fmt::format("Total called: {}", total_called.load()));
  // // 5. Read data from client
  // char buffer[BUFFER_SIZE] = {0};
  // read(socket, buffer, BUFFER_SIZE);
  // LOG_INFO(fmt::format("Client: {}", buffer));

  // // 6. Send response to client
  // const char *hello = "Hello from server!";
  // send(socket, hello, strlen(hello), 0);
  // LOG_INFO("Message sent to client");
  // close(socket);
}