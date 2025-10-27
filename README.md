# My Server Project

A modern C++20 concurrent server application demonstrating asynchronous networking with a custom thread pool framework.

## Overview

This project implements a production-ready TCP server with configurable thread pool management, structured logging, and an executor pattern for pluggable connection handling. Built on modern C++20 features with emphasis on thread safety and clean architecture.

## Project Structure

```
.
├── my-async-framework/    # Core asynchronous framework
│   ├── scheduling/        # Thread pool and queues
│   └── logging/           # Thread-safe logging system
├── my-server/             # Server application
│   └── executors/         # Custom connection handlers
└── build/                 # Build output directory
```

## Components

### my-async-framework

Core infrastructure providing:

- **Server**: TCP server accepting connections on configurable ports
- **Thread Pool**: Worker threads executing tasks concurrently
- **Unbounded SPMC Queue**: Lock-free task distribution
- **Logging System**: Millisecond-precision, thread-safe logging with compile-time level filtering

See [`my-async-framework/README.md`](my-async-framework/README.md) for details.

### my-server

Application layer demonstrating:

- Connection acceptance and delegation
- Custom executor implementations
- Configuration management

See [`my-server/README.md`](my-server/README.md) for details.

## Requirements

- **C++20** compiler (clang++ or g++)
- **CMake 3.10+**
- **POSIX** system (macOS, Linux)

## Building

### Manual Build

```bash
mkdir build && cd build
cmake .. -DDEBUG_MODE=ON
cmake --build .
./my-server/my-server-bin
```

### Using VS Code

Press `F8` (Code Runner) to build and run automatically with debug mode enabled.

### Debug vs Release

- **Debug mode** (`-DDEBUG_MODE=ON`): Includes `LOG_DEBUG` messages, debug symbols
- **Release mode** (default): Only INFO/ERROR/WARNING logs, optimized binary

## Configuration

Edit `my-async-framework/static_settings.hpp` to configure:

### Log Levels

```cpp
#define LOG_LEVEL_DEBUG    // Most verbose (includes thread pool internals)
#define LOG_LEVEL_INFO     // Standard logging (default)
#define LOG_LEVEL_ERROR    // Errors only
```

### Network Settings

```cpp
#define PORT 8081           // Server listening port
```

## Usage Example

```cpp
#include <my-async-framework/server.hpp>
#include <my-server/executors/simple-executor.hpp>

int main() {
  MyAsyncFramework::Server server(SimpleExecutor);
  server.ListenAndServe();  // Blocks until Ctrl+C
  return 0;
}
```

## Features

- ✅ **Thread-safe**: All operations are synchronized with mutexes/atomics
- ✅ **Asynchronous**: Non-blocking connection handling via thread pool
- ✅ **Configurable**: Adjust worker threads, log levels, ports
- ✅ **Observable**: Structured logging with timestamps
- ✅ **Graceful shutdown**: SIGINT handling for clean termination
- ✅ **Modern C++**: Uses C++20 features and best practices

## Dependencies

- **fmt** (v10.1.1): String formatting library, downloaded automatically by CMake
- **C++ Standard Library**: Threading, networking, chrono
- **POSIX sockets**: `socket`, `bind`, `listen`, `accept`

## Running the Server

```bash
cd build
cmake ..
make
./my-server/my-server-bin
```

Server listens on `localhost:8081` by default. Press `Ctrl+C` for graceful shutdown.

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
│          Unbounded SPMC Queue            │
└────────────┬─────────────────────────────┘
              │
              ▼
    ┌─────────┴─────────┐
    │                   │
    ▼                   ▼
┌────────┐         ┌────────┐
│Worker 1│         │Worker N│  ← Thread Pool
└────┬───┘         └────┬───┘
     │                  │
     └──────────┬───────┘
                ▼
      ┌──────────────────┐
      │  Custom Executor  │
      └───────────────────┘
```

## Logging

Logs include millisecond timestamps and are thread-safe:

```
[14:23:45.123] [INFO] Server listening on port 8081
[14:23:46.456] [DEBUG] Executor 0 executing task
[14:23:46.789] [INFO] Handling connection 4
```
