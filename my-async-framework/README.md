# My Async Framework

A lightweight C++20 asynchronous networking framework that provides thread pool management, connection handling, and structured logging for building concurrent server applications.

## Overview

This framework implements a production-ready server infrastructure with the following components:

- **Server**: TCP server that accepts connections and delegates work to a thread pool
- **Thread Pool**: Configurable pool of worker threads (default: 8) that execute tasks concurrently
- **Unbounded MPMC Queue**: Lock-free queue for task distribution across workers
- **Logging System**: Thread-safe logging with millisecond timestamps and compile-time log level filtering
- **Synchronization Primitives**: Custom mutex and condition variable implementations

## Features

- Thread-safe asynchronous task execution
- Flexible executor pattern for custom business logic
- Configurable number of worker threads (default: 8)
- Signal handling (SIGINT) for graceful shutdown
- Structured logging with DEBUG/WARNING/INFO/ERROR levels
- Millisecond-precision timestamps in log messages
- Lock-free task queue for efficient task distribution

## Building and Installation

### Build

```bash
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
```

### Install

```bash
cmake --install build --prefix ~/local/my-async-framework
```

This installs the library to `~/local/my-async-framework/lib` and headers to `~/local/my-async-framework/include`.

### Using in Your Project

After installation, use the framework in your CMake project:

```cmake
find_package(MyAsyncFramework REQUIRED CONFIG)

add_executable(YourServer
    main.cpp
)

target_link_libraries(YourServer
    PRIVATE
        MyAsyncFramework::MyAsyncFramework
)
```

## Usage

### Basic Server

Create a server with your executor function:

```cpp
#include <my-async-framework/server.hpp>

void MyExecutor(const int socket) {
    // Your custom logic here
    // Process client connection on the given socket
    close(socket);
}

int main() {
    MyAsyncFramework::Server server(MyExecutor, 8080);
    server.ListenAndServe();  // Blocks until Ctrl+C
    return 0;
}
```

### Server API

```cpp
namespace MyAsyncFramework {

class Server {
public:
    // Constructor takes executor function and port
    Server(ExecutorFunctionType executor, const int port);

    // Start listening and serving (blocks until SIGINT)
    void ListenAndServe();

    // Destructor closes server socket
    ~Server();
};

// Executor function type
using ExecutorFunctionType = std::function<void(const int)>;
}
```

### Thread Pool

The server uses a `ThreadPool` internally. By default, it creates 8 worker threads. The thread pool:

- Automatically starts when the server is constructed
- Uses an unbounded MPMC (Multiple Producer, Multiple Consumer) queue
- Gracefully shuts down when `Stop()` is called

To customize the thread pool size, modify `kWorkerThreads` in `scheduling/thread_pool.hpp` or extend the `Server` class.

## Logging

### Configuration

Configure log level via CMake when building:

```bash
cmake -S . -B build -DLOG_LEVEL=DEBUG    # Most verbose
cmake -S . -B build -DLOG_LEVEL=INFO     # Standard logging
cmake -S . -B build -DLOG_LEVEL=WARNING   # Warnings and errors
cmake -S . -B build -DLOG_LEVEL=ERROR     # Errors only
```

Available levels: `DEBUG`, `WARNING`, `INFO`, `ERROR`. Default is `DEBUG` if not specified.

### Usage

```cpp
#include <my-async-framework/logging/logging.hpp>

LOG_DEBUG("Debug message");
LOG_INFO("Info message");
LOG_WARNING("Warning message");
LOG_ERROR("Error message");
```

Log messages are thread-safe and include timestamps in `HH:MM:SS.mmm` format:

```
[14:23:45.123] [DEBUG] Executor 0 executing his task
[14:23:45.456] [INFO] Server listening on port 8080
```

## Architecture

### Server Flow

1. **Initialization**: Server creates socket, binds to port, starts thread pool
2. **Accept Loop**: Main thread accepts connections using `select()` with timeout
3. **Task Queueing**: Each accepted connection is wrapped in a `Worker` and added to the queue
4. **Task Execution**: Worker threads pop tasks from the queue and execute the executor function
5. **Shutdown**: On SIGINT, server stops accepting new connections and waits for workers to finish

### Thread Pool

- **Queue**: Unbounded MPMC queue (lock-free)
- **Workers**: Configurable number of worker threads (default: 8)
- **Tasks**: Each task is a `Worker` object containing the executor function and socket

### Synchronization

The framework uses custom synchronization primitives:

- **Mutex**: Custom mutex implementation (see `sync/mutex.hpp`)
- **Condition Variable**: Custom condition variable (see `sync/condition_variable.hpp`)
- **Atomic Operations**: Used in the lock-free queue

## Testing

The framework includes comprehensive unit tests. See [`tests/README.md`](tests/README.md) for details on building and running tests.

## Dependencies

- **fmt** (v10.1.1): String formatting library, automatically downloaded via CMake FetchContent
- **C++20 Standard Library**: Threading, networking, chrono
- **POSIX sockets**: `socket`, `bind`, `listen`, `accept`, `select`

## Configuration

### Static Settings

Edit `static_settings.hpp` to configure:

```cpp
// Server accept timeout (seconds) for checking should_stop flag
#define SERVER_ACCEPT_TIMEOUT_SECONDS 1

// Listen queue size
#define LISTEN_QUEUE_SIZE 1024
```

## Examples

See [`../services/simple-tcp-server`](../services/simple-tcp-server) for a complete example of using the framework.
