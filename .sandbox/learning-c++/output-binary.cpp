#include <iostream>

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

void OutputString(const std::string s) {
  std::cout << "--------------" << std::endl << std::endl;
  
  for (size_t i = 0; i < s.size() + 1; ++i) {
    const char c = s[i];
    std::cout << c << ": ";
    OutputBinary(c);
  }
} 

int main() {
  for (int i = 0; i < 17; ++i) {
    OutputBinary(i);
  }
  OutputString("abcdefg");
  OutputString("i l0ve ca0s");
}