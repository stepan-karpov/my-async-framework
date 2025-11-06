#include <iostream>
#include <deque>
#include <thread>
#include <condition_variable>

struct Base {
  Base(int x) : x_(x) { std::cout << "Base constr\n"; }
  Base(const Base& other) { x_ = other.x_; std::cout << "Base copy constr\n"; assert(false && "Base copy constr called"); }
  Base(Base&& other) { x_ = other.x_; std::cout << "Base move constr\n"; }
  Base& operator=(const Base& other) { x_ = other.x_; std::cout << "Base copy operator=\n"; return *this; }
  Base& operator=(Base&& other) { x_ = other.x_; std::cout << "Base move operator=\n"; return *this; }
  int x_ = 0;
};

std::ostream& operator<<(std::ostream& stream, const Base& base) {
  stream << base.x_;
  return stream;
}

// Bounded Multiple Producers Multiple Consumers Pipe
template <typename T, size_t kMaxSize = 3>
class Pipe {
public:

  template <typename U>
  void Send(U&& t) {
    std::unique_lock lock(mutex_);
    not_full_.wait(lock, [this]{
      return underlying_.size() < kMaxSize;
    });
    underlying_.push_back(std::forward<U>(t));
    not_empty_.notify_one();
  }

  T Recv() {
    std::unique_lock lock(mutex_);
    not_empty_.wait(lock, [this]{
      return underlying_.size() > 0;
    });
    auto front = std::move(underlying_.front());
    underlying_.pop_front();
    not_full_.notify_one();
    return front;
  }

  // TODO: think of adding Close()

private:
  std::deque<T> underlying_;
  std::mutex mutex_;
  std::condition_variable not_empty_;
  std::condition_variable not_full_;
};

void producer(Pipe<Base>& pipe) {
  for (int i = 0; i < 10; ++i) {
    pipe.Send(Base(42 + i));
    std::cout << "All send\n";
  }
  
}

void consumer(Pipe<Base>& pipe) {
  for (int i = 0; i < 10; ++i) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    const auto t = pipe.Recv();
    std::cout << "got value = " << t << "\n";
  }
}

int main() {
  Pipe<Base> pipe;
  std::thread t1(producer, std::ref(pipe));
  std::thread t2(consumer, std::ref(pipe));

  t1.join();
  t2.join();
}