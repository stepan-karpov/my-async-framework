#pragma once

#include <functional>
#include <netinet/in.h>  // For sockaddr_in

#include "scheduling/thread_pool.hpp"

#define DEFAULT_PORT 8082
#define LISTEN_QUEUE_SIZE 1024

namespace MyAsyncFramework {

// Takes sockaddr_in and file descriptor number
using ExecutorFunctionType = std::function<void(const int)>;
using ThreadPool = scheduling::ThreadPool;

class Server {
public:
  Server() = delete;
  Server(ExecutorFunctionType executor);  
  Server(const int port, ExecutorFunctionType executor);

  Server(const Server&) = delete;
  Server(Server&&) = delete;
  Server& operator=(const Server&) = delete;
  Server& operator=(Server&&) = delete;

  ~Server();

  // Locks parent process until SIGINT Signal
  void ListenAndServe();
private:
  struct ServerInfo {
    int server_fd;
    sockaddr_in address;
  };

  ServerInfo InitializeServer();
  void Listen();
  void ListenDebug();

  ThreadPool thread_pool_;

  const int kPort_;
  const ServerInfo kServerInfo_;
  ExecutorFunctionType executor_;
};

} // namespace MyAsyncFramework
