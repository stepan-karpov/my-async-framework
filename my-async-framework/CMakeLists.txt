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

add_executable(test-mutex sync/tests/test_mutex.cpp)
target_link_libraries(test-mutex PRIVATE my-async-framework GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test-mutex)