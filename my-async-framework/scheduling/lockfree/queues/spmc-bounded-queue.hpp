#pragma once

#include <atomic>
#include <memory>
#include <cassert>
#include <optional>

template <typename T, typename Allocator = std::allocator<T>>
class SPMCBoundedQueue {
 private:
  struct Node {
    std::atomic<int64_t> sequence;
    std::optional<T> value;
  };
 private:
  using AllocTTraits = std::allocator_traits<Allocator>;
  using AllocatorNode = AllocTTraits::template rebind_alloc<Node>;
  using AllocNodeTraits = std::allocator_traits<AllocatorNode>;
 private:
  [[ no_unique_address ]] AllocatorNode allocator_;
  Node* buffer_;
  alignas(64) const int64_t capacity_;
  alignas(64) int64_t head_{0};
  alignas(64) std::atomic<int64_t> tail_{0};
 public:
  SPMCBoundedQueue(int64_t capacity, Allocator allocator = Allocator())
    : allocator_(allocator)
      , buffer_(AllocNodeTraits::allocate(allocator_, capacity))
      , capacity_(capacity)
  {
    if (capacity <= 0 || (capacity & (capacity - 1)) != 0) {
      AllocNodeTraits::deallocate(allocator_, buffer_, capacity);
      throw std::invalid_argument{"capacity must be power of 2"};
    }
    for (int64_t i = 0; i < capacity_; ++i) {
      AllocNodeTraits::construct(allocator_, buffer_ + i, i, std::nullopt);
    }
  }

  bool Push(T object) {
    int64_t current_head = head_;
    Node& node = buffer_[current_head & (capacity_ - 1)];

    int64_t current_sequence = node.sequence.load(std::memory_order::acquire);

    if (current_sequence - current_head < 0) {
      return false;
    }

    node.value = std::move(object);
    node.sequence.store(current_head + 1, std::memory_order::release);

    head_ = current_head + 1;
    return true;
  }

  std::optional<T> Pop() {
    while (true) {
      int64_t current_tail = tail_.load(std::memory_order::relaxed);
      Node& node = buffer_[current_tail & (capacity_ - 1)];

      int64_t current_sequence = node.sequence.load(std::memory_order::acquire);
      int64_t diff = current_sequence - (current_tail + 1);

      if (diff == 0) {
        if (tail_.compare_exchange_weak(
            current_tail, 
            current_tail + 1,
            std::memory_order::relaxed,
            std::memory_order::relaxed    
        )) {
          auto value = std::move(node.value.value());
          node.value.reset();
          node.sequence.store(current_tail + capacity_, std::memory_order::release);
          return value;
        }
      } else if (diff < 0) {
        return std::nullopt;
      }
    }
  }

  ~SPMCBoundedQueue() {
    for (int64_t i = 0; i < capacity_; ++i) {
      AllocNodeTraits::destroy(allocator_, buffer_ + i);
    }
    AllocNodeTraits::deallocate(allocator_, buffer_, capacity_);
  }
};