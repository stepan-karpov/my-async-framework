#include "context.hpp"

namespace sure::machine {

// https://eli.thegreenplace.net/2011/09/06/stack-frame-layout-on-x86-64/
static void MachineContextTrampoline(void*, void*, void*, void*, void*, void*, void* arg7) {
  sure::ITrampoline* t = (sure::ITrampoline*)arg7;
  t->Run();
}

void MachineContext::Setup(sure::StackView stack, sure::ITrampoline* trampoline) {
  rsp_ = SetupMachineContext((void*)&stack.back(), (void*)MachineContextTrampoline, (void*)trampoline);
}

}  // namespace sure::machine

