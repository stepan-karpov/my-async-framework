#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <unistd.h>      // For close()
#include <sys/socket.h>  // For sockets
#include <netinet/in.h>  // For sockaddr_in

#include <fmt/format.h>

#include "static_settings.hpp"

#include <my-async-framework/server.hpp>


int total_called = 0;

void Executor(const int socket) {
  LOG_INFO(fmt::format("Handling connection {}", socket));
  std::this_thread::sleep_for(std::chrono::seconds(10));
  ++total_called;
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

int main() {
  MyAsyncFramework::Server server(Executor);

  server.ListenAndServe();

  LOG_INFO(fmt::format("Total called: {}", total_called));

  return 0;
}