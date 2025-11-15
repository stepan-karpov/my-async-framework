#include <span>
#include <vector>
#include <cstddef>
#include <iostream>

void ProcessData(std::span<std::byte> buffer) {
    for (auto b : buffer) {
      std::cout << std::bitset<8>(std::to_integer<int>(b)) << "\n";
    }
}

int main() {
    std::vector<std::byte> raw_data(10);
    
    for (int i = 9; i >= 0; --i) {
      raw_data[i] = std::byte(9 - i);
    }

    ProcessData(std::span<std::byte>(raw_data.data(), raw_data.size()));
}