#pragma once

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <atomic>
#include <optional>

// Lockfree algorithm
// Default values initialization
// Memory models
// Allocators vs owning memory
// ABA problem
// Use-After-Free problem

template <typename T, typename Allocator = std::allocator<T>>
class SPSCBoundedQueue {
 private:
  [[ no_unique_address ]] Allocator allocator_;
  alignas(64) T* buffer_;
  alignas(64) const uint64_t capacity_;
  alignas(64) std::atomic<uint64_t> head_;
  alignas(64) std::atomic<uint64_t> tail_;
 private:
  using AllocatorTraits = std::allocator_traits<Allocator>;
 public:
  SPSCBoundedQueue(const size_t capacity, Allocator allocator = Allocator())
  : 
    allocator_(allocator)
    , buffer_(AllocatorTraits::allocate(allocator_, capacity))
    , capacity_(capacity)
    , head_(0)
    , tail_(0)
  {
    if (capacity == 0 || (capacity & (capacity - 1)) != 0) {
      AllocatorTraits::deallocate(allocator_, buffer_, capacity);
      throw std::invalid_argument{"not pow 2"};
    }
  }

  bool Push(T object) {
    uint64_t current_head = head_.load(std::memory_order::relaxed);

    if (current_head - tail_.load(std::memory_order::acquire) >= capacity_) {
      return false;
    }

    auto ptr = buffer_ + (current_head & (capacity_ - 1));
    AllocatorTraits::construct(
      allocator_,
      ptr,
      std::move(object)
    );
    head_.store(current_head + 1 , std::memory_order::release);
    return true;
  }

  std::optional<T> Pop() {
    uint64_t current_tail = tail_.load(std::memory_order::relaxed);

    if (current_tail == head_.load(std::memory_order::acquire)) {
      return std::nullopt;
    }

    auto ptr = buffer_ + (current_tail & (capacity_ - 1));
    auto return_value = std::move(*ptr);

    AllocatorTraits::destroy(
      allocator_,
      ptr
    );

    tail_.store(current_tail + 1, std::memory_order::release);
    return return_value;
  }

  ~SPSCBoundedQueue() {
    if (!buffer_) { return; }
    while (Pop().has_value()) {}
    AllocatorTraits::deallocate(allocator_, buffer_, capacity_);
  }
};