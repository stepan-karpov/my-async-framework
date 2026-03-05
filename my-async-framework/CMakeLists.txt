cmake_minimum_required(VERSION 3.19)
project(MyAsyncFramework LANGUAGES CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Fetching dependencies
include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.1.1
)
FetchContent_MakeAvailable(fmt)

file(GLOB_RECURSE MYASYNCFRAMEWORK_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)
file(GLOB_RECURSE MYASYNCFRAMEWORK_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.S"
)
list(FILTER MYASYNCFRAMEWORK_SOURCES EXCLUDE REGEX "/tests/")

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

include(CTest)

if(BUILD_TESTING)
    message(STATUS "Configuring unit tests")
    add_subdirectory(tests)
endif()