#pragma once

#include <functional>
#include <netinet/in.h>  // For sockaddr_in

#include <my-async-framework/scheduling/fiber/thread_pool/thread_pool.hpp>
#include <my-async-framework/scheduling/fiber/core/body.hpp>

#define LISTEN_QUEUE_SIZE 1024

namespace MyAsyncFramework {

// Takes sockaddr_in and file descriptor number
using ExecutorFunctionType = MyAsyncFramework::scheduling::fiber::core::Body;
using ThreadPool = scheduling::fiber::thread_pool::ThreadPool;

class Server {
public:
  Server() = delete;
  Server(ExecutorFunctionType executor, const int port);

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
  void ListenMacOs();

  ThreadPool thread_pool_;

  const int kPort_;
  const ServerInfo kServerInfo_;
  ExecutorFunctionType executor_;
};

} // namespace MyAsyncFramework
