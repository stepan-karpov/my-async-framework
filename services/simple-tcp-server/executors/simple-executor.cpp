#include "simple-executor.hpp"

#include <thread>
#include <chrono>
#include <unistd.h>      // For close()
#include <sys/socket.h>  // For sockets
#include <netinet/in.h>  // For sockaddr_in

#include <my-async-framework/logging/logging.hpp>
#include <my-async-framework/network/tcp/read.hpp>
#include <my-async-framework/network/tcp/send.hpp>

namespace MyServer::executors {

const int kSimpleExecutorBufferSize = 12;

void SimpleExecutor(Args&& args) {
  const int fd = args.descriptor_;
  for (int i = 0; i < 50000; ++i) {
    // 5. Read data from client
    char buffer[kSimpleExecutorBufferSize] = {0};
    MyAsyncFramework::network::tcp::Read(fd, buffer, kSimpleExecutorBufferSize);
    size_t sum = 0;
    for (const char c : buffer) { sum += c; }

    std::string response_str = std::to_string(sum);
    MyAsyncFramework::network::tcp::Send(fd, response_str.c_str(), strlen(response_str.c_str()), 0);
  }
  close(fd);
}

} // namespace MyServer::executors