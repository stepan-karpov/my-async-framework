#pragma once

namespace sure {

// All runnable entities (coroutines, fibers)
// should implement ITrampoline

struct ITrampoline {
  // Never returns
  virtual void Run() noexcept = 0;

 protected:
  ~ITrampoline() = default;
};

}  // namespace sure

