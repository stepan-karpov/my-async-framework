// #include <iostream>
// #include <string>
// #include <unistd.h>
// #include <cstring>
// #include <vector>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <fcntl.h>
// #include <sys/epoll.h>
// #include <unordered_map>

// #define PORT 8082
// #define BUFFER_SIZE 12
// #define MAX_EVENTS 64

// struct ClientState {
//     int cycles_left = 50000;
// };

// bool set_nonblocking(int fd) {
//     int flags = fcntl(fd, F_GETFL, 0);
//     if (flags == -1)
//         return false;
//     if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
//         return false;
//     return true;
// }

// // Возвращает true, если соединение надо закрыть
// bool handle_client(int fd, ClientState& state) {
//     char buffer[BUFFER_SIZE];
//     while (state.cycles_left > 0) {
//         ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE);
//         if (bytes_read > 0) {
//             size_t sum = 0;
//             for (int j = 0; j < bytes_read; ++j)
//                 sum += static_cast<unsigned char>(buffer[j]);
//             std::string response_str = std::to_string(sum);
//             ssize_t bytes_sent = send(fd, response_str.c_str(), response_str.size(), 0);
//             if (bytes_sent == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
//                 perror("send");
//                 return true;
//             }
//             state.cycles_left--;
//         } else if (bytes_read == 0) {
//             // Клиент закрыл соединение
//             return true;
//         } else {
//             if (errno == EAGAIN || errno == EWOULDBLOCK)
//                 break; // Данные пока закончились
//             perror("read");
//             return true;
//         }
//     }
//     if (state.cycles_left == 0)
//         return true;
//     return false;
// }

// int main() {
//     int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd == -1) {
//         perror("socket failed");
//         return 1;
//     }

//     int opt = 1;
//     setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

//     sockaddr_in address;
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(PORT);

//     if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
//         perror("bind failed");
//         close(server_fd);
//         return 1;
//     }

//     if (listen(server_fd, 20) < 0) {
//         perror("listen failed");
//         close(server_fd);
//         return 1;
//     }
//     std::cout << "Server listening on port " << PORT << std::endl;

//     if (!set_nonblocking(server_fd)) {
//         perror("nonblocking failed");
//         close(server_fd);
//         return 1;
//     }

//     int epoll_fd = epoll_create1(0);
//     if (epoll_fd == -1) {
//         perror("epoll_create1 failed");
//         close(server_fd);
//         return 1;
//     }

//     struct epoll_event event;
//     event.data.fd = server_fd;
//     event.events = EPOLLIN;

//     if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
//         perror("epoll_ctl: listen_sock");
//         close(epoll_fd);
//         close(server_fd);
//         return 1;
//     }

//     std::unordered_map<int, ClientState> clients;
//     struct epoll_event events[MAX_EVENTS];

//     while (true) {
//         int n_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
//         if (n_fds == -1) {
//             if (errno == EINTR)
//                 continue;
//             perror("epoll_wait");
//             break;
//         }

//         for (int i = 0; i < n_fds; ++i) {
//             int fd = events[i].data.fd;

//             if (fd == server_fd) {
//                 // Новое соединение
//                 while (true) {
//                     sockaddr_in cli_addr;
//                     socklen_t cli_len = sizeof(cli_addr);
//                     int client_fd = accept(server_fd, (sockaddr*)&cli_addr, &cli_len);
//                     if (client_fd == -1) {
//                         if (errno == EAGAIN || errno == EWOULDBLOCK)
//                             break;
//                         perror("accept");
//                         continue;
//                     }

//                     std::cout << "New connection accepted! fd=" << client_fd << std::endl;

//                     if (!set_nonblocking(client_fd)) {
//                         perror("nonblock client sock");
//                         close(client_fd);
//                         continue;
//                     }

//                     epoll_event cevent;
//                     cevent.data.fd = client_fd;
//                     cevent.events = EPOLLIN | EPOLLET;
//                     if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &cevent) == -1) {
//                         perror("epoll_ctl: client_sock");
//                         close(client_fd);
//                         continue;
//                     }
//                     clients[client_fd] = ClientState{};
//                 }
//             } else {
//                 bool need_close = handle_client(fd, clients[fd]);
//                 if (need_close) {
//                     std::cout << "Closing connection fd=" << fd << std::endl;
//                     epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
//                     close(fd);
//                     clients.erase(fd);
//                 }
//             }
//         }
//     }

//     close(epoll_fd);
//     close(server_fd);
//     return 0;
// }
