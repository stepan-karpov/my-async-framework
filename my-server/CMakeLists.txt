add_executable(
  my-server-bin
  main.cpp
  executors/simple-executor.cpp
)


target_link_libraries(my-server-bin PUBLIC my-async-framework)