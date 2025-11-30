#pragma once

#include <functional>

namespace MyAsyncFramework::scheduling::fiber::core {

struct Args {
  const int descriptor_;
};

using Body = std::function<void(Args&&)>;

} // namespace MyAsyncFramework::scheduling::fiber::core