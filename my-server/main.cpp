#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include <fmt/format.h>

#include <my-async-framework/server.hpp>

#include <my-server/static_settings.hpp>
#include <my-server/executors/simple-executor.hpp>

int main() {
  MyAsyncFramework::Server server(SimpleExecutor);

  server.ListenAndServe();
  return 0;
}