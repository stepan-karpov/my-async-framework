#pragma once

#include <functional>

namespace MyAsyncFramework::scheduling::fiber::thread_pool {

using Worker = std::function<void()>;

} // namespace MyAsyncFramework::scheduling::fiber::thread_pool