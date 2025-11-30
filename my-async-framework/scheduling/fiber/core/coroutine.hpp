#pragma once

#include <functional>

#include "execution_context/context.hpp"
#include "execution_context/stack_view.hpp"
#include "execution_context/trampoline.hpp"

#include "body.hpp"

namespace MyAsyncFramework::scheduling::fiber::core {

constexpr size_t kStackSize = 1 << 17; // 128kB

struct Coroutine : private sure::ITrampoline {
 public:
  template <typename F>
  Coroutine(Args&& args, F&& function)
    : args_(std::move(args)),
      function_(std::forward<F>(function)),
      stack_storage_(kStackSize),
      stack_(stack_storage_.data(), stack_storage_.size()) {
    coro_context_.Setup(stack_, this);
  }

  void Resume();
  void Suspend();
  bool IsDone();

 private:
  void Run() noexcept final;

 private:
  Args args_;
  Body function_;
  std::vector<std::byte> stack_storage_;
  sure::StackView stack_;
  sure::ExecutionContext coro_context_;
  sure::ExecutionContext main_context_;
  bool is_done_ = false;
};

} // namespace MyAsyncFramework::scheduling::fiber::core