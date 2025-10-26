cmake_minimum_required(VERSION 3.10)

# Set project parameters
project(MyServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for IDE
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Download fmt library
include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.1.1
)
FetchContent_MakeAvailable(fmt) 

# Add library framework
add_library(async_framework 
  my-async-framework/server.cpp
  my-async-framework/scheduling/thread_pool.cpp
)
target_include_directories(async_framework PUBLIC .)
target_link_libraries(async_framework PUBLIC fmt::fmt)

# Add main file
add_executable(server main.cpp)

target_link_libraries(server PUBLIC async_framework fmt::fmt)



