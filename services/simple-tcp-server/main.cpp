#include <iostream>
#include <string>

#include <fmt/format.h>

#include <my-async-framework/server.hpp>
#include <my-async-framework/static_settings.hpp>

#include "executors/simple-executor.hpp"
#include "executors/debug-executor.hpp"

#define Executor MyServer::executors::SimpleExecutor

int main() {
  MyAsyncFramework::Server server(Executor);

  server.ListenAndServe();
  return 0;
}