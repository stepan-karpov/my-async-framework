#include <future>
#include <iostream>
#include <thread>
 
void foo(std::promise<int>& p) {
  p.set_value_at_thread_exit(9);
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  std::cout << "exit\n";
}

void example1() {
  std::promise<int> p;
  std::future<int> f = p.get_future();
  std::thread t(foo, std::ref(p));
  t.detach();
  
  std::cout << "Waiting..." << std::flush;
  f.wait();
  std::cout << "Done!\nResults are: " << f.get() << '\n';
}


void bar(std::promise<int>& p) {
  p.set_value(9);
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  std::cout << "exit\n";
}

void example2() {
  std::promise<int> p;
  std::future<int> f = p.get_future();
  std::thread t(bar, std::ref(p));
  t.detach();
  
  std::cout << "Waiting..." << std::flush;
  f.wait();
  std::cout << "Done!\nResults are: " << f.get() << '\n';
}

void example3() {
  std::future<int> f;
  {
    std::promise<int> p;
    f = p.get_future();
  }
  
  std::cout << "Waiting..." << std::flush;
  f.wait();
  std::cout << "Done!\nResults are: " << f.get() << '\n';
}

int main() {
  // example1();
  // example2();
  example3();
}