#include "coroutine.hpp"

namespace MyAsyncFramework::scheduling::fiber::core {

void Coroutine::Suspend() {
  coro_context_.SwitchTo(main_context_);
}

void Coroutine::Resume() {
  main_context_.SwitchTo(coro_context_);
}

bool Coroutine::IsDone() {
  return is_done_;
}

void Coroutine::Run() noexcept {
  function_(std::move(args_));
  is_done_ = true;
  coro_context_.ExitTo(main_context_);
}

} // namespace MyAsyncFramework::scheduling::fiber::core