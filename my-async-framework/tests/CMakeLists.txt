message(STATUS "Configuring unit tests")

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(TEST_SOURCES
    sync/test_mutex.cpp
    sync/test_condition_variable.cpp
    sync/queues/unbounded_mpmc_queue_test.cpp
    scheduling/fiber/thread_pool/test_thread_pool.cpp
    scheduling/fiber/core/test_coroutine.cpp
    scheduling/fiber/core/test_fiber.cpp
)

foreach(source IN LISTS TEST_SOURCES)
    cmake_path(GET source STEM name)
    set(target "test-${name}")

    add_executable(${target} "${source}")

    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
    target_link_libraries(${target} PRIVATE GTest::gtest_main MyAsyncFramework)
    target_compile_definitions(${target} PRIVATE LOG_LEVEL=${LOG_LEVEL})
    target_compile_options(${target} PRIVATE -fsanitize=address,undefined)
    target_link_options   (${target} PRIVATE -fsanitize=address,undefined)

    add_test(NAME ${target} COMMAND ${target})
    set_tests_properties(${target} PROPERTIES TIMEOUT 60)
endforeach()