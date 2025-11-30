#include "read.hpp"

namespace MyAsyncFramework::network::tcp {

void Read(int descriptor, void* buffer, size_t buffer_size) {
  read(descriptor, buffer, buffer_size);
  // TODO:
  // 1. Create non-blocking WaitGroup
  // 2. Add fiber with read
  // 3. WaitGroup.Wait()
}

} // namespace MyAsyncFramework::network::tcp