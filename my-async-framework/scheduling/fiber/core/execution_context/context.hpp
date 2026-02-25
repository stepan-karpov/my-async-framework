#pragma once

#include "trampoline.hpp"
#include "stack_view.hpp"

// Platform detection macros
#if defined(__x86_64__) || defined(_M_X64) || defined(__amd64__)
  #define SURE_PLATFORM_X86_64
  #include "machine/x86_64/context.hpp"
#else
  #error "Unsupported platform. Only x86_64 and ARM64 are supported."
#endif

namespace sure {

// Lightweight Execution Context
// Only includes MachineContext (no sanitizers, no exceptions)

class ExecutionContext {
 public:
  // Empty context, cannot be a target for SwitchTo
  ExecutionContext() = default;

  // Non-copyable
  ExecutionContext(const ExecutionContext&) = delete;
  ExecutionContext& operator=(const ExecutionContext&) = delete;

  // Non-movable
  ExecutionContext(ExecutionContext&&) = delete;
  ExecutionContext& operator=(ExecutionContext&&) = delete;

  // Prepare execution context for running trampoline->Run()
  // on stack `stack`
  void Setup(StackView stack, ITrampoline* trampoline) {
    impl_.Setup(stack, trampoline);
  }

  // Symmetric Control Transfer
  // 1) Save current execution context to `this`
  // 2) Activate `target` context
  void SwitchTo(ExecutionContext& target) {
    impl_.SwitchTo(target.impl_);
  }

  // Leave current execution context forever
  // Last context switch in ITrampoline::Run
  [[noreturn]] void ExitTo(ExecutionContext& target) {
    impl_.SwitchTo(target.impl_);
    __builtin_unreachable();
  }

  // For overflow checking
  // Calling StackPointer on non-suspended ExecutionContext results in UB
  void* StackPointer() const noexcept {
    return impl_.StackPointer();
  }

 private:
  sure::machine::MachineContext impl_;
};

}  // namespace sure

