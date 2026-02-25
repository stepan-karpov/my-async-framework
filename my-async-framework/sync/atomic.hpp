#pragma once

#include <atomic>

namespace MyAsyncFramework::sync {

// TODO: add custom atomic using asm
using Atomic = typename std::atomic<uint32_t>;

}