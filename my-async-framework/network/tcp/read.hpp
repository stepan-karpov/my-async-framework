#pragma once

#include <cstddef>
#include <unistd.h>

namespace MyAsyncFramework::network::tcp {

void Read(int descriptor, void* buffer, size_t buffer_size);

} // namespace MyAsyncFramework::network::tcp