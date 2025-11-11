# Services

This directory contains server applications built on top of the My Async Framework. This guide explains how to create and set up a new service.

## Prerequisites

Ensure the framework is installed:

```bash
cd ../my-async-framework
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
cmake --install build --prefix ~/local/my-async-framework
```

## Creating a New Service

### 1. Create Directory and CMakeLists.txt

```bash
mkdir your-service-name
cd your-service-name
```

Create `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.10)
project(YourServiceName LANGUAGES CXX)

find_package(MyAsyncFramework REQUIRED CONFIG)

add_executable(YourServiceName
    main.cpp
)

target_compile_features(YourServiceName PRIVATE cxx_std_20)

target_link_libraries(YourServiceName
    PRIVATE
        MyAsyncFramework::MyAsyncFramework
)
```

### 2. Create main.cpp

```cpp
#include <my-async-framework/server.hpp>
#include <unistd.h>
#include <sys/socket.h>

void YourExecutor(const int socket) {
    // Your business logic
    char buffer[1024] = {0};
    read(socket, buffer, sizeof(buffer) - 1);

    // Process and respond...
    const char* response = "Hello!";
    send(socket, response, strlen(response), 0);

    close(socket);
}

int main() {
    MyAsyncFramework::Server server(YourExecutor, 8080);
    server.ListenAndServe();
    return 0;
}
```

### 3. Build and Run

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=~/local/my-async-framework -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
./build/YourServiceName
```

## Optional: Organize with Executors

Create `executors/my-executor.hpp`:

```cpp
#pragma once
namespace MyService::executors {
void MyExecutor(const int socket);
}
```

Create `executors/my-executor.cpp`:

```cpp
#include "executors/my-executor.hpp"
#include <unistd.h>

namespace MyService::executors {
void MyExecutor(const int socket) {
    // Implementation
    close(socket);
}
}
```

Update `main.cpp`:

```cpp
#include "executors/my-executor.hpp"
// ...
MyAsyncFramework::Server server(MyService::executors::MyExecutor, 8080);
```

Add to `CMakeLists.txt`:

```cmake
add_executable(YourServiceName
    main.cpp
    executors/my-executor.cpp
)
```

## Example

See `simple-tcp-server/` for a complete working example.
