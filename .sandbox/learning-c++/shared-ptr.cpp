#include <iostream>
#include <memory>

struct Base {
  int my_value = 42;
};

struct StructTwo {
  StructTwo(const std::shared_ptr<Base>& ptr) : common_ptr_(ptr) {}
  std::shared_ptr<Base> common_ptr_;
};


struct StructOne {
  StructOne() : common_ptr_(std::make_shared<Base>()) {}
  StructTwo Allocate() {
    return StructTwo(common_ptr_);
  }
  std::shared_ptr<Base> common_ptr_;
};

int main() {
  StructOne s1;
  auto t = s1.Allocate();
  std::cout << "Here\n";
}