# My Server Project

A modern C++20 concurrent server application demonstrating asynchronous networking with a custom thread pool framework.

## Overview

This project implements a production-ready TCP server with configurable thread pool management, structured logging, and an executor pattern for pluggable connection handling. Built on modern C++20 features with emphasis on thread safety and clean architecture.

## Project Structure

```
.
├── my-async-framework/    # Core asynchronous framework library
│   ├── scheduling/        # Thread pool and queues
│   ├── logging/           # Thread-safe logging system
│   ├── sync/              # Synchronization primitives (mutex, condition variable)
│   └── tests/             # Unit tests
├── services/              # Server applications
    ├── simple-tcp-server/  # Example TCP server implementation
    │   └── executors/     # Custom connection handlers
    └── my-client/         # Go client for testing
```

## Components

### my-async-framework

Core infrastructure library providing:

- **Server**: TCP server accepting connections on configurable ports
- **Thread Pool**: Configurable pool of worker threads (default: 8) executing tasks concurrently
- **Unbounded MPMC Queue**: Lock-free queue for task distribution across workers
- **Logging System**: Millisecond-precision, thread-safe logging with compile-time level filtering
- **Synchronization Primitives**: Custom mutex and condition variable implementations

See [`my-async-framework/README.md`](my-async-framework/README.md) for details.

### services/simple-tcp-server

Example server application demonstrating:

- Connection acceptance and delegation
- Custom executor implementations (SimpleExecutor, DebugExecutor)
- Integration with the framework

See [`services/README.md`](services/README.md) for details.

## Requirements

- **C++20** compiler (clang++ or g++)
- **CMake 3.19+** (for framework) or **CMake 3.10+** (for services)
- **POSIX** system (macOS, Linux)
- **fmt** library (v10.1.1) - downloaded automatically by CMake

## Building

### Building the Framework

First, build and install the framework:

```bash
cd my-async-framework
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
cmake --install build --prefix ~/local/my-async-framework
```

### Building a Service

After installing the framework, build a service:

```bash
cd services/simple-tcp-server
cmake -S . -B build -DCMAKE_PREFIX_PATH=~/local/my-async-framework -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
./build/SimpleTCPServer
```

### Using VS Code

Press `F8` (Code Runner) to build and run automatically. The script will:

1. Build and install the framework
2. Build the simple-tcp-server service
3. Run the server

## Configuration

### Log Levels

Configure log level via CMake when building the framework:

```bash
cmake -S . -B build -DLOG_LEVEL=DEBUG    # Most verbose
cmake -S . -B build -DLOG_LEVEL=INFO     # Standard logging
cmake -S . -B build -DLOG_LEVEL=WARNING  # Warnings and errors
cmake -S . -B build -DLOG_LEVEL=ERROR    # Errors only
```

Available levels: `DEBUG`, `WARNING`, `INFO`, `ERROR`. Default is `DEBUG`.

### Server Port

Configure the server port in your service's `main.cpp`:

```cpp
#define PORT 8082
MyAsyncFramework::Server server(Executor, PORT);
```

### Thread Pool Size

The thread pool defaults to 8 worker threads. To customize, modify `kWorkerThreads` in `my-async-framework/scheduling/thread_pool.hpp` or pass a custom value to the `ThreadPool` constructor.

## Usage Example

```cpp
#include <my-async-framework/server.hpp>
#include "executors/simple-executor.hpp"

int main() {
  MyAsyncFramework::Server server(MyServer::executors::SimpleExecutor, 8082);
  server.ListenAndServe();  // Blocks until Ctrl+C
  return 0;
}
```

## Features

- ✅ **Thread-safe**: All operations are synchronized with custom mutexes/atomics
- ✅ **Asynchronous**: Non-blocking connection handling via thread pool
- ✅ **Configurable**: Adjust worker threads, log levels, ports
- ✅ **Observable**: Structured logging with millisecond timestamps
- ✅ **Graceful shutdown**: SIGINT handling for clean termination
- ✅ **Modern C++**: Uses C++20 features and best practices
- ✅ **Tested**: Comprehensive unit tests for core components

## Dependencies

- **fmt** (v10.1.1): String formatting library, downloaded automatically by CMake via FetchContent
- **C++ Standard Library**: Threading, networking, chrono
- **POSIX sockets**: `socket`, `bind`, `listen`, `accept`
- **GoogleTest** (v1.14.0): For unit tests (optional, enabled with `BUILD_TESTING=ON`)

## Running the Server

```bash
# Build and install framework
cd my-async-framework
cmake -S . -B build
cmake --build build
cmake --install build --prefix ~/local/my-async-framework

# Build and run service
cd ../services/simple-tcp-server
cmake -S . -B build -DCMAKE_PREFIX_PATH=~/local/my-async-framework
cmake --build build
./build/SimpleTCPServer
```

Server listens on `localhost:8082` by default (configurable in `main.cpp`). Press `Ctrl+C` for graceful shutdown.

## Architecture

```
┌─────────────────┐
│  Client Socket  │
└────────┬────────┘
         │
         ▼
┌─────────────────────────────────────────┐
│          Server (main thread)           │
│  ┌───────────────────────────────────┐  │
│  │  Accept connections → Queue tasks │  │
│  └────────────┬──────────────────────┘  │
└─────────────┬────────────────────────────┘
              │
              ▼
┌──────────────────────────────────────────┐
│      Unbounded MPMC Queue (lock-free)      │
└────────────┬─────────────────────────────┘
              │
              ▼
    ┌─────────┴─────────┐
    │                   │
    ▼                   ▼
┌────────┐         ┌────────┐
│Worker 1│  ...   │Worker 8│  ← Thread Pool (default: 8 threads)
└────┬───┘         └────┬───┘
     │                  │
     └──────────┬───────┘
                ▼
      ┌──────────────────┐
      │  Custom Executor │  ← Your business logic
      └───────────────────┘
```

## Logging

Logs include millisecond timestamps and are thread-safe:

```
[14:23:45.123] [INFO] Server listening on port 8082
[14:23:46.456] [DEBUG] Executor 0 executing his task
[14:23:46.789] [DEBUG] Connection accepted!
```

## Testing

The framework includes comprehensive unit tests. See [`my-async-framework/tests/README.md`](my-async-framework/tests/README.md) for details on building and running tests.
