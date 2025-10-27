#pragma once


namespace MyServer::executors {
/*
Simple executor that receives a text message and sends "Hello from server!" to the client.
*/
void DebugExecutor(const int socket);

} // namespace MyServer::executors