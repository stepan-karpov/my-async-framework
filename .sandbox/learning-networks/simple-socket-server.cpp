// Also see epoll realization for Linux in my-async-framework/server.cpp

#include <iostream>
#include <string>
#include <unistd.h>      // For close()
#include <sys/socket.h>  // For sockets
#include <netinet/in.h>  // For sockaddr_in

#define PORT 8082
#define BUFFER_SIZE 12

template <typename T>
void OutputBinary(const T& x) {
  const int object_size = sizeof(x);

  for (int i = object_size * 8 - 1; i >= 0; --i) {
    int32_t mask = (1 << i);
    bool bit = (mask & x) > 0;
    std::cout << bit;
  }
  std::cout << std::endl;
}

int main() {
    // 1. Create server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        return 1;
    }

    // 2. Bind socket to IP address and port
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    address.sin_port = htons(PORT);       // htons to convert port to network byte order

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return 1;
    }

    // 3. Start listening for incoming connections
    std::cout << "Server listening on port " << PORT << std::endl;
    if (listen(server_fd, 20) < 0) { // 20 - maximum number of queued connections
        perror("listen failed");
        close(server_fd);
        return 1;
    }

    while (true) {
        // 4. Accept incoming connection (blocking call)
        int addrlen = sizeof(address);
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept failed");
            close(server_fd);
            return 1;
        }
        std::cout << "Connection accepted!" << std::endl;
        
        for (int i = 0; i < 50000; ++i) {
            // 5. Read data from client
            char buffer[BUFFER_SIZE] = {0};
            read(new_socket, buffer, BUFFER_SIZE);
            size_t sum = 0;
            for (const char c : buffer) { sum += c; }
    
            std::string response_str = std::to_string(sum);
            send(new_socket, response_str.c_str(), strlen(response_str.c_str()), 0);
        }
        close(new_socket);
    }

    // 7. Close sockets
    close(server_fd);

    return 0;
}
