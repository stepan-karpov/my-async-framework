#pragma once

#include <my-async-framework/scheduling/fiber/core/body.hpp>

namespace MyServer::executors {

using Args = MyAsyncFramework::scheduling::fiber::core::Args;

/*
Simple executor that receives a text message and sends "Hello from server!" to the client.
*/
void SimpleExecutor(Args&& args);

} // namespace MyServer::executors