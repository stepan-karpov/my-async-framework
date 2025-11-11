# cmake -S . -B build -DCMAKE_PREFIX_PATH=~/local/my-async-framework -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
# cmake --build build

cmake_minimum_required(VERSION 3.10)

project(SimpleTCPServer LANGUAGES CXX)

find_package(MyAsyncFramework REQUIRED CONFIG)

add_executable(SimpleTCPServer
    main.cpp
    executors/simple-executor.cpp
    executors/debug-executor.cpp
)

target_compile_features(SimpleTCPServer PRIVATE cxx_std_20)


target_link_libraries(SimpleTCPServer
    PRIVATE
        MyAsyncFramework::MyAsyncFramework
)