# cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
# cmake --build build
# cmake --install build --prefix ~/local/my-async-framework

cmake_minimum_required(VERSION 3.19)

project(MyAsyncFramework LANGUAGES CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.1.1
)
FetchContent_MakeAvailable(fmt)

file(GLOB_RECURSE MYASYNCFRAMEWORK_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Add platform-specific assembly files and exclude platform-specific C++ files
# Only compile the assembly file for the current platform to avoid conflicts
# The C++ code uses preprocessor defines (__x86_64__, __aarch64__) to select headers,
# but we need to compile only the matching assembly file to avoid linker conflicts
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|amd64|i686")
    # For x86_64, include x86_64 assembly and exclude ARM64 C++ files
    file(GLOB_RECURSE X86_64_ASM_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/scheduling/fiber/core/execution_context/machine/x86_64/*.S"
    )
    list(APPEND MYASYNCFRAMEWORK_SOURCES ${X86_64_ASM_SOURCES})
    list(FILTER MYASYNCFRAMEWORK_SOURCES EXCLUDE REGEX "armv8_a_64/context\\.cpp")
    message(STATUS "Building for x86_64 platform")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64")
    # For ARM64, include ARM64 assembly and exclude x86_64 C++ files
    file(GLOB_RECURSE ARM64_ASM_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/scheduling/fiber/core/execution_context/machine/armv8_a_64/*.S"
    )
    list(APPEND MYASYNCFRAMEWORK_SOURCES ${ARM64_ASM_SOURCES})
    list(FILTER MYASYNCFRAMEWORK_SOURCES EXCLUDE REGEX "x86_64/context\\.cpp")
    message(STATUS "Building for ARM64 platform")
else()
    message(WARNING "Unknown platform: CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}")
    message(WARNING "Defaulting to x86_64. If this is incorrect, please set CMAKE_SYSTEM_PROCESSOR explicitly.")
    file(GLOB_RECURSE X86_64_ASM_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/scheduling/fiber/core/execution_context/machine/x86_64/*.S"
    )
    list(APPEND MYASYNCFRAMEWORK_SOURCES ${X86_64_ASM_SOURCES})
    list(FILTER MYASYNCFRAMEWORK_SOURCES EXCLUDE REGEX "armv8_a_64/context\\.cpp")
endif()

list(FILTER MYASYNCFRAMEWORK_SOURCES EXCLUDE REGEX "/tests/")
list(FILTER MYASYNCFRAMEWORK_SOURCES EXCLUDE REGEX "test_coroutine\\.cpp")

add_library(MyAsyncFramework STATIC ${MYASYNCFRAMEWORK_SOURCES})

target_include_directories(MyAsyncFramework
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(MyAsyncFramework PUBLIC fmt::fmt)

install(TARGETS MyAsyncFramework
    EXPORT MyAsyncFrameworkTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install fmt separately (fmt::fmt is an alias, need to install the real target)
# fmt usually provides its own install, but if needed, can be installed manually
# For this, use install(EXPORT) for fmt or install fmt separately

file(GLOB_RECURSE MYASYNCFRAMEWORK_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

foreach(HEADER ${MYASYNCFRAMEWORK_HEADERS})
    file(RELATIVE_PATH REL_HEADER ${CMAKE_CURRENT_SOURCE_DIR} ${HEADER})
    get_filename_component(HEADER_DIR ${REL_HEADER} DIRECTORY)
    install(FILES ${HEADER}
        DESTINATION include/my-async-framework/${HEADER_DIR}
    )
endforeach()

install(EXPORT MyAsyncFrameworkTargets
    FILE MyAsyncFrameworkTargets.cmake
    NAMESPACE MyAsyncFramework::
    DESTINATION lib/cmake/MyAsyncFramework
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    cmake/MyAsyncFrameworkConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/MyAsyncFrameworkConfig.cmake
    INSTALL_DESTINATION lib/cmake/MyAsyncFramework
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/MyAsyncFrameworkConfig.cmake
    DESTINATION lib/cmake/MyAsyncFramework
)

option(BUILD_TESTING "Build tests" ON)

if(BUILD_TESTING)
    # Load GoogleTest only if tests are needed
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
    
    # Configure log level (required parameter)
    # LOG_LEVEL should be a numeric value: 0=DEBUG, 1=WARNING, 2=INFO, 3=ERROR
    if(NOT DEFINED LOG_LEVEL)
        message(FATAL_ERROR "LOG_LEVEL is required. Please specify: -DLOG_LEVEL=0|1|2|3")
    endif()
    
    # Create test executables
    add_executable(test-mutex tests/sync/test_mutex.cpp)
    add_executable(test-condition-variable tests/sync/test_condition_variable.cpp)
    add_executable(test-unbounded-mpmc-queue tests/sync/queues/unbounded_mpmc_queue_test.cpp)
    add_executable(test-thread-pool tests/scheduling/fiber/thread_pool/test_thread_pool.cpp)
    add_executable(test-coroutine tests/scheduling/fiber/core/test_coroutine.cpp)
    add_executable(test-fiber tests/scheduling/fiber/core/test_fiber.cpp)
    
    # List of test executables
    set(TEST_EXECUTABLES
        test-mutex
        test-condition-variable
        test-unbounded-mpmc-queue
        test-thread-pool
        test-coroutine
        test-fiber
    )
    
    # List of test executables that should NOT use sanitizers
    # (e.g., tests that use low-level assembly or context switching)
    set(TEST_EXECUTABLES_NO_SANITIZER
        test-coroutine
        test-fiber
    )
    
    # Configure each test
    foreach(test_target IN LISTS TEST_EXECUTABLES)
        target_include_directories(${test_target} PRIVATE 
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/..
        )
        # Explicitly add GTest include directories for IDE
        get_target_property(GTEST_INCLUDE_DIRS GTest::gtest INTERFACE_INCLUDE_DIRECTORIES)
        if(GTEST_INCLUDE_DIRS)
            target_include_directories(${test_target} PRIVATE ${GTEST_INCLUDE_DIRS})
        endif()
        target_compile_definitions(${test_target} PUBLIC LOG_LEVEL=${LOG_LEVEL})
        
        # Link libraries in correct order:
        # GTest::gtest_main already includes GTest::gtest, so specify only it
        # MyAsyncFramework should be after GTest, as tests use the framework
        target_link_libraries(${test_target} PRIVATE 
            GTest::gtest_main
            MyAsyncFramework
        )
        
        # Explicitly add pthread for macOS (required for futex_like.hpp)
        if(APPLE OR UNIX)
            find_package(Threads REQUIRED)
            target_link_libraries(${test_target} PRIVATE Threads::Threads)
        endif()

        # Apply sanitizers only if SANITIZER is defined and test is not in exclusion list
        if(DEFINED SANITIZER)
            list(FIND TEST_EXECUTABLES_NO_SANITIZER ${test_target} IS_EXCLUDED)
            if(IS_EXCLUDED EQUAL -1)
                # Test is not in exclusion list, apply sanitizers
                target_compile_options(${test_target} PRIVATE -fsanitize=${SANITIZER})
                target_link_options(${test_target} PRIVATE -fsanitize=${SANITIZER})
            else()
                message(STATUS "Sanitizers disabled for ${test_target}")
            endif()
        endif()
    endforeach()
    
    # Enable GoogleTest support
    include(GoogleTest)
    enable_testing()
    
    # Automatic test discovery
    gtest_discover_tests(test-mutex
        PROPERTIES TIMEOUT 60
    )
    gtest_discover_tests(test-condition-variable
        PROPERTIES TIMEOUT 60
    )
    gtest_discover_tests(test-unbounded-mpmc-queue
        PROPERTIES TIMEOUT 60
    )
    gtest_discover_tests(test-thread-pool
        PROPERTIES TIMEOUT 60
    )
    gtest_discover_tests(test-coroutine
        PROPERTIES TIMEOUT 60
    )
    gtest_discover_tests(test-fiber
        PROPERTIES TIMEOUT 60
    )
endif()