#include <iostream>
#include <string>

#include <fmt/format.h>

#include <my-async-framework/server.hpp>
#include <my-async-framework/static_settings.hpp>

#include "executors/simple-executor.hpp"
#include "executors/debug-executor.hpp"

#define Executor MyServer::executors::SimpleExecutor
#define PORT 8082

int main() {
  MyAsyncFramework::Server server(Executor, PORT);

  server.ListenAndServe();
  return 0;
}