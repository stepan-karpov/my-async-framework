cmake_minimum_required(VERSION 3.10)

# Set project parameters
project(MyProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Sanitizer support
# Usage: cmake .. -DSANITIZER=address|thread|undefined|address,undefined
if(DEFINED SANITIZER)
  set(SANITIZER_FLAGS "-fsanitize=${SANITIZER}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZER_FLAGS}")
  message(STATUS "Sanitizers enabled: ${SANITIZER}")
endif()

# Download fmt library
include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.1.1
)
FetchContent_MakeAvailable(fmt)

# Download Google Test
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest) 

add_subdirectory(my-server)
add_subdirectory(my-async-framework)
