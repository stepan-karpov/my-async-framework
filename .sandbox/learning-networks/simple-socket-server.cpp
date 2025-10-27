#include <iostream>
#include <string>
#include <unistd.h>      // Для close()
#include <sys/socket.h>  // Для сокетов
#include <netinet/in.h>  // Для sockaddr_in

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
    // 1. Создаем сокет-сервер
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        return 1;
    }

    // 2. Привязываем сокет к IP-адресу и порту
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Слушаем на всех интерфейсах
    address.sin_port = htons(PORT);       // htons для преобразования порта в сетевой формат

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return 1;
    }

    // 3. Начинаем слушать входящие соединения
    std::cout << "Сервер слушает порт " << PORT << std::endl;
    if (listen(server_fd, 20) < 0) { // 3 - максимальное число соединений в очереди
        perror("listen failed");
        close(server_fd);
        return 1;
    }

    while (true) {
        // 4. Принимаем входящее соединение (это блокирующий вызов)
        int new_socket;
        int addrlen = sizeof(address);
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            close(server_fd);
            return 1;
        }
        std::cout << "Соединение принято!" << std::endl;
        
        for (int i = 0; i < 50000; ++i) {
            // 5. Читаем данные от клиента
            char buffer[BUFFER_SIZE] = {0};
            read(new_socket, buffer, BUFFER_SIZE);
            size_t sum = 0;
            for (const char c : buffer) { sum += c; }
    
            std::string response_str = std::to_string(sum);
            send(new_socket, response_str.c_str(), strlen(response_str.c_str()), 0);
        }
        close(new_socket);
    }

    // 7. Закрываем сокеты
    close(server_fd);

    return 0;
}
