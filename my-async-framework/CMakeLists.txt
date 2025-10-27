add_library(
  my-async-framework
  server.cpp
  scheduling/thread_pool.cpp
)

target_include_directories(my-async-framework
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}  # Add parent directory for <my-async-framework/...> includes
)

target_link_libraries(my-async-framework PUBLIC fmt::fmt)