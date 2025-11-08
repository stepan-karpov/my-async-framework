#pragma once

#include <atomic>

namespace MyAsyncFramework::sync {

// TODO: add custom atomic using asm (and be able to stay both macOs/Linux compatible)
using Atomic = typename std::atomic<uint32_t>;

}