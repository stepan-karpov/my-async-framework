// nasm -f elf64 sum.asm -o sum.o && clang++ sum.cpp sum.o -o sum.trash && rm sum.o && ./sum.trash

#include <iostream>

extern "C" int sum(int64_t a, int64_t b);

int main() {
    int64_t x = 7, y = 5;
    std::cout << "sum(" << x << ", " << y << ") = " << sum(x, y) << std::endl;
    return 0;
}
