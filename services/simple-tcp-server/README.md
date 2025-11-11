# My Server

A concurrent TCP server built on top of the My Async Framework, demonstrating how to implement custom connection handlers using the executor pattern.

## Overview

This server application showcases practical use of:

- Multi-threaded task execution via thread pools
- Non-blocking connection handling
- Custom executor implementations for business logic
- Configurable logging and debugging

## Architecture

The server consists of three main components:

- **Main Entry Point** (`main.cpp`): Initializes the server with a custom executor
- **Executors** (`executors/`): Pluggable handlers for processing client connections
- **Framework Config** (`../my-async-framework/static_settings.hpp`): Centralized configuration

## Building

```bash
cd build
cmake ..
cmake --build .
./my-server-bin
```

Or use VS Code's Code Runner (F8) to build and run automatically.

## Configuring Log Level

Edit `static_settings.hpp` to set the desired verbosity:

```cpp
#define LOG_LEVEL_DEBUG  // See all messages
#define LOG_LEVEL_INFO   // Production-level logs
#define LOG_LEVEL_ERROR  // Errors only
```

## Default Port

The server listens on port `8081` by default. Configure via the `Server` constructor:

```cpp
Server server(8080, MyExecutor);  // Custom port
```

## Executors

Executors are functions that process client connections. See `executors/README.md` for details on implementing custom handlers.
