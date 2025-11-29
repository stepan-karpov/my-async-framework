#pragma once

#include "../../trampoline.hpp"
#include "../../stack_view.hpp"

extern "C" {
#include "context.h"
}

namespace sure::machine {

// Target architecture: x86-64

struct MachineContext {
  void* rsp_;

  void Setup(sure::StackView stack, sure::ITrampoline* trampoline);

  void SwitchTo(MachineContext& target) {
    SwitchMachineContext(&rsp_, &target.rsp_);
  }

  static constexpr bool kStackPointerAvailable = true;

  void* StackPointer() const noexcept {
    return rsp_;
  }
};

}  // namespace sure::machine

