#include "server.hpp"

#include <iostream>
#include <string>
#include <unistd.h>      // For close()
#include <sys/socket.h>  // For sockets
#include <signal.h>      // For signal handling
#include <atomic>        // For interrupt flag
#include <thread>        // For sleep_for

using namespace MyAsyncFramework;

// Global flag for tracking Ctrl+C press
namespace {

std::atomic<bool> should_stop(false);

void signalHandler(int signum) {
  (void)signum;
  should_stop = true;
  LOG_DEBUG("Interrupt signal received, shutting down...");
}

} // namespace

Server::Server(ExecutorFunctionType executor)
  : thread_pool_(),
    kPort_(DEFAULT_PORT), 
    kServerInfo_(InitializeServer()),
    executor_(executor) {}

Server::Server(const int port, ExecutorFunctionType executor)
  : thread_pool_(),
    kPort_(port),
    kServerInfo_(InitializeServer()),
    executor_(executor) {}

Server::~Server() {
  close(kServerInfo_.server_fd);
  LOG_DEBUG("Server destructor called");
}

Server::ServerInfo Server::InitializeServer() {
  Server::ServerInfo server_info;
  
  // 1. Create server socket
  server_info.server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_info.server_fd == -1) {
    perror("socket failed");
    throw std::runtime_error("Failed to initialize socket");
  }

  // 2. Bind socket to IP address and port
  server_info.address.sin_family = AF_INET;
  server_info.address.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
  server_info.address.sin_port = htons(kPort_);       // htons to convert port to network byte order

  if (bind(server_info.server_fd, (struct sockaddr *)&server_info.address, sizeof(server_info.address)) < 0) {
    perror("bind failed");
    close(server_info.server_fd);
    throw std::runtime_error("Failed to bind socket to PORT=" + std::to_string(kPort_));
  }
  
  LOG_DEBUG("Socket created and binded");
  return server_info;
}

void Server::ListenAndServe() {
  // Register SIGINT (Ctrl+C) signal handler
  signal(SIGINT, signalHandler);
  
  LOG_DEBUG(fmt::format("Server listening on port {}. Press Ctrl+C to stop", kPort_));
  
  // 3. Start listening for incoming connections
  if (listen(kServerInfo_.server_fd, LISTEN_QUEUE_SIZE) < 0) { // LISTEN_QUEUE_SIZE - maximum number of queued connections
    perror("listen failed");
    close(kServerInfo_.server_fd);
    throw std::runtime_error("Failed to start listening");
  }

  const int addrlen = sizeof(kServerInfo_.address);

  const int kDummyTasks = 15;
  int k = 0;

  while (!should_stop) {
    ++k;
    if (k >= kDummyTasks) { should_stop = true; }
    LOG_DEBUG("checking...");

    // 4. Accept incoming connection (this is a blocking call)
    // int new_socket;
    // if ((new_socket = accept(kServerInfo_.server_fd, (struct sockaddr *)&kServerInfo_.address, (socklen_t*)&addrlen)) < 0) {
    //   perror("accept failed");
    //   throw std::runtime_error("An error occurred while accepting socket connection");
    // }
    LOG_DEBUG("Connection accepted!");
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // for debugging
    scheduling::Worker worker(executor_, 1);
    thread_pool_.AddTask(std::move(worker));

    // executor_(new_socket);  // Call the executor function
  }

  thread_pool_.Stop();

  LOG_INFO("Server stopped");
}