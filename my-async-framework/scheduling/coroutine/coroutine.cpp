#include "coroutine.hpp"

namespace MyAsyncFramework::scheduling::coroutine {

void Coroutine::Run() noexcept {
  function_(SuspendContext(this));
  is_done_ = true;
  coro_context_.ExitTo(main_context_);
}

void Coroutine::Suspend() {
  coro_context_.SwitchTo(main_context_);
}

void Coroutine::Resume() {
  main_context_.SwitchTo(coro_context_);
}

bool Coroutine::IsDone() {
  return is_done_;
}

} // namespace MyAsyncFramework::scheduling::coroutine