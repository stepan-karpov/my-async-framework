#pragma once

#include <cstddef>
#include <unistd.h>

namespace MyAsyncFramework::network::tcp {

void Send(int descriptor, const void* buffer, size_t buffer_size, int flags);

} // namespace MyAsyncFramework::network::tcp