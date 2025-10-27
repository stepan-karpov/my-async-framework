#include <iostream>
#include <string>


#include <fmt/format.h>

#include <my-async-framework/server.hpp>
#include <my-async-framework/static_settings.hpp>

#include <my-server/executors/simple-executor.hpp>
#include <my-server/executors/debug-executor.hpp>

#ifdef DEBUG_MODE
#define Executor MyServer::executors::DebugExecutor
#else
#define Executor MyServer::executors::SimpleExecutor
#endif

int main() {
  MyAsyncFramework::Server server(Executor);

  server.ListenAndServe();
  return 0;
}