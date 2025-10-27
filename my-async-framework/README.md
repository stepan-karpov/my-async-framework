# My Async Framework

A lightweight C++20 asynchronous networking framework that provides thread pool management, connection handling, and structured logging for building concurrent server applications.

## Overview

This framework implements a production-ready server infrastructure with the following components:

- **Server**: HTTP-like server that accepts connections and delegates work to a thread pool
- **Thread Pool**: Configurable pool of worker threads that execute tasks concurrently
- **Unbounded SPMC Queue**: Lock-free queue for task distribution across workers
- **Logging System**: Thread-safe logging with millisecond timestamps and compile-time log level filtering

## Features

- Thread-safe asynchronous task execution
- Flexible executor pattern for custom business logic
- Configurable number of worker threads
- Signal handling (SIGINT) for graceful shutdown
- Structured logging with DEBUG/WARNING/INFO/ERROR levels
- Millisecond-precision timestamps in log messages

## Usage

Include the framework in your CMake project:

```cmake
target_link_libraries(your_target PUBLIC my-async-framework)
```

Create a server with your executor:

```cpp
#include <my-async-framework/server.hpp>

void MyExecutor(const int socket) {
  // Your custom logic here
}

int main() {
  MyAsyncFramework::Server server(MyExecutor);
  server.ListenAndServe();
  return 0;
}
```

## Logging

Configure log level in `static_settings.hpp`:

```cpp
#define LOG_LEVEL_DEBUG  // Most verbose
#define LOG_LEVEL_INFO   // Medium verbose
#define LOG_LEVEL_ERROR  // Least verbose
```

Log messages include timestamps in `HH:MM:SS.mmm` format and are thread-safe.
