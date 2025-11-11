#include "simple-executor.hpp"

#include <thread>
#include <chrono>
#include <unistd.h>      // For close()
#include <sys/socket.h>  // For sockets
#include <netinet/in.h>  // For sockaddr_in

#include <my-async-framework/logging/logging.hpp>

namespace MyServer::executors {

const int kSimpleExecutorBufferSize = 12;

void SimpleExecutor(const int socket) {
  for (int i = 0; i < 50000; ++i) {
    // 5. Read data from client
    char buffer[kSimpleExecutorBufferSize] = {0};
    read(socket, buffer, kSimpleExecutorBufferSize);
    size_t sum = 0;
    for (const char c : buffer) { sum += c; }

    std::string response_str = std::to_string(sum);
    send(socket, response_str.c_str(), strlen(response_str.c_str()), 0);
  }
  close(socket);
}

} // namespace MyServer::executors