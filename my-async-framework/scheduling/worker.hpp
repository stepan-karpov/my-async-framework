#pragma once

#include <functional>

namespace MyAsyncFramework::scheduling {

class Worker {
public:
  Worker(const std::function<void(const int)>& func, const int fd) : func_(func), fd_(fd) {}
  
  Worker(const Worker&) = delete;
  Worker& operator=(const Worker&) = delete;

  Worker(Worker&&) = default;
  Worker& operator=(Worker&&) = default;

  void operator()() {
    func_(fd_);
  }
private:
  std::function<void(const int)> func_;
  int fd_;
};

};