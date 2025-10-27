#include "simple-executor.hpp"

#include <thread>
#include <chrono>
#include <unistd.h>      // For close()
#include <sys/socket.h>  // For sockets
#include <netinet/in.h>  // For sockaddr_in

#include <my-async-framework/logging/logging.hpp>

namespace MyServer::executors {

const int kSimpleExecutorBufferSize = 1024;

void SimpleExecutor(const int socket) {
  // 5. Read data from client
  char buffer[kSimpleExecutorBufferSize] = {0};
  read(socket, buffer, kSimpleExecutorBufferSize);
  LOG_INFO(fmt::format("Client: {}", buffer));

  // 6. Send response to client
  const char *hello = "Hello from server!";
  send(socket, hello, strlen(hello), 0);
  LOG_INFO("Message sent to client");
  close(socket);
}

} // namespace MyServer::executors