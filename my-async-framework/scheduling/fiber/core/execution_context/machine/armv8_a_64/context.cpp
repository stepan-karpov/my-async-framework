#include "context.hpp"

namespace sure::machine {

static void MachineContextTrampoline(void*, void*, void*, void*, void*, void*, void*, void*, void* arg9) {
  sure::ITrampoline* t = (sure::ITrampoline*)arg9;
  t->Run();
}

void MachineContext::Setup(sure::StackView stack, sure::ITrampoline* trampoline) {
  rsp_ = SetupMachineContext((void*)&stack.back(), (void*)MachineContextTrampoline, (void*)trampoline);
}

void MachineContext::SwitchTo(MachineContext& target) {
  SwitchMachineContext(&rsp_, &target.rsp_);
}

}  // namespace sure::machine

