#pragma once

#include <functional>
#include <my-async-framework/scheduling/coroutine/execution_context/context.hpp>
#include <my-async-framework/scheduling/coroutine/execution_context/stack_view.hpp>
#include <my-async-framework/scheduling/coroutine/execution_context/trampoline.hpp>

namespace MyAsyncFramework::scheduling::coroutine {

constexpr size_t kStackSize = 1 << 18; // 64kB

struct Coroutine : sure::ITrampoline {
 public:
  class SuspendContext {
   public:
    SuspendContext(Coroutine* coro) : coro_(coro) {}
    void Suspend() { coro_->Suspend(); }

   private:
    Coroutine* coro_;
  };
 private:
  friend class SuspendContext;
  void Suspend();

  void CoroutineStartup();
  using Function = std::function<void(SuspendContext)>;
 public:
  template <typename F>
  Coroutine(F&& function)
    : function_(std::forward<Function>(function)),
      stack_storage_(kStackSize),
      stack_(stack_storage_.data(), stack_storage_.size()) {
    coro_context_.Setup(stack_, this);
  }

  void Resume();
  void Run() noexcept final;
  bool IsDone();

 private:
 Function function_;
  std::vector<std::byte> stack_storage_;
  sure::StackView stack_;
  sure::ExecutionContext coro_context_;
  sure::ExecutionContext main_context_;
  bool is_done_ = false;
};

} // namespace MyAsyncFramework::scheduling::coroutine