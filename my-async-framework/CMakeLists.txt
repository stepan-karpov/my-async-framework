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

# Log level configuration (can be overridden via -DLOG_LEVEL=ERROR)
if(NOT DEFINED LOG_LEVEL)
  set(LOG_LEVEL "DEBUG")
endif()

# Determine log level macro name
if(LOG_LEVEL STREQUAL "ERROR")
  set(LOG_LEVEL_MACRO "LOG_LEVEL_ERROR")
elseif(LOG_LEVEL STREQUAL "INFO")
  set(LOG_LEVEL_MACRO "LOG_LEVEL_INFO")
elseif(LOG_LEVEL STREQUAL "WARNING")
  set(LOG_LEVEL_MACRO "LOG_LEVEL_WARNING")
else()
  set(LOG_LEVEL_MACRO "LOG_LEVEL_DEBUG")
endif()

# Apply log level to library
target_compile_definitions(my-async-framework PUBLIC ${LOG_LEVEL_MACRO})

# sync tests
add_executable(test-mutex sync/tests/test_mutex.cpp)
add_executable(test-condition-variable sync/tests/test_condition_variable.cpp)
add_executable(test-unbounded-mpmc-queue sync/queues/tests/unbounded_mpmc_queue_test.cpp)
# scheduling tests
add_executable(test-thread-pool scheduling/tests/test_thread_pool.cpp)

# List of all test executables
set(TEST_EXECUTABLES
  test-mutex
  test-condition-variable
  test-unbounded-mpmc-queue
  test-thread-pool
)

# Apply log level and link libraries to all test executables
foreach(test_target IN LISTS TEST_EXECUTABLES)
  target_link_libraries(${test_target} PRIVATE my-async-framework GTest::gtest GTest::gtest_main)
  target_compile_definitions(${test_target} PUBLIC ${LOG_LEVEL_MACRO})
endforeach()


include(GoogleTest)
gtest_discover_tests(test-mutex)
gtest_discover_tests(test-condition-variable)
gtest_discover_tests(test-unbounded-mpmc-queue)
gtest_discover_tests(test-thread-pool)