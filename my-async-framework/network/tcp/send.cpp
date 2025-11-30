#include "send.hpp"

#include <sys/socket.h>  // For sockets

namespace MyAsyncFramework::network::tcp {

void Send(int descriptor, const void* buffer, size_t buffer_size, int flags) {
  send(descriptor, buffer, buffer_size, flags);
  // TODO:
  // 1. Create non-blocking WaitGroup
  // 2. Add fiber with send
  // 3. WaitGroup.Wait()
}

} // namespace MyAsyncFramework::network::tcp