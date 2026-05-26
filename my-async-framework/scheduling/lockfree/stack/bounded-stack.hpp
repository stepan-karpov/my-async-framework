#pragma once

#include <cstdint>
#include <optional>
#include <memory>
#include <atomic>
#include <cassert>

#include "../allocators/free-list-lock-free-allocator.hpp"

// Lockfree algorithm
// Default values initialization
// Memory models
// Allocators vs owning memory
// ABA problem
// Use-After-Free problem

template <typename T>
class BoundedStack {
 private:
  struct Node {
    T value;
    Node* last_value_ptr = nullptr;

    Node(T&& value)
      : value(std::move(value))
        , last_value_ptr(nullptr)
    {}
  };
 private:
  using TaggedPtr = std::uintptr_t;
  using TagType = uint16_t;

  inline static TaggedPtr Pack(Node* ptr, TagType tag) {
    return reinterpret_cast<TaggedPtr>(ptr) |  (static_cast<TaggedPtr>(tag) << 48);
  }

  inline static Node* UnpackPtr(TaggedPtr tagged_ptr) {
    return reinterpret_cast<Node*>(tagged_ptr & 0x0000FFFFFFFFFFFF);
  }

  inline static TagType UnpackTag(TaggedPtr tagged_ptr) {
    return static_cast<TagType>(tagged_ptr >> 48);
  }
 private:
  using AllocatorType = FreeListLockfreeAllocator<Node>;
  using AllocatorTraits = std::allocator_traits<AllocatorType>;
 private:
  alignas(64) std::atomic<TaggedPtr> head_;
  alignas(64) AllocatorType allocator_;
 public:
  BoundedStack(const size_t capacity)
    : head_(Pack(nullptr, 0)) 
      , allocator_(capacity)
  {}

  bool Push(T object) {
    Node* new_head = AllocatorTraits::allocate(allocator_, 1);
    if (new_head == nullptr) { return false; }
    AllocatorTraits::construct(allocator_, new_head, std::move(object));

    while (true) {
      TaggedPtr current_head_tagged = head_.load(std::memory_order::release);
      Node* current_head = UnpackPtr(current_head_tagged);
      TagType new_tag = UnpackTag(current_head_tagged) + 1;

      new_head->last_value_ptr = current_head;

      if (head_.compare_exchange_weak(
          current_head_tagged,
          Pack(new_head, new_tag),
          std::memory_order::release,
          std::memory_order::relaxed
      )) {
        return true;
      }
    }
  }

  std::optional<T> Pop() {
    Node* current_head;
    while (true) {
      TaggedPtr current_head_tagged = head_.load(std::memory_order::acquire);
      current_head = UnpackPtr(current_head_tagged);
      TagType new_tag = UnpackTag(current_head_tagged) + 1;

      if (current_head == nullptr) { return std::nullopt; }
      
      Node* new_head = current_head->last_value_ptr;
      
      if (head_.compare_exchange_weak(
          current_head_tagged,
          Pack(new_head, new_tag),
          std::memory_order::acquire,
          std::memory_order::relaxed
      )) {
        break;
      }
    }

    auto return_value = std::move(current_head->value);
    AllocatorTraits::destroy(allocator_, current_head);
    AllocatorTraits::deallocate(allocator_, current_head, 1);
    return return_value;
  }

  ~BoundedStack() {
    Node* current_head = UnpackPtr(head_.load(std::memory_order_relaxed));
    while (current_head != nullptr) {
      Node* next_head = current_head->last_value_ptr;
      AllocatorTraits::destroy(allocator_, current_head);
      AllocatorTraits::deallocate(allocator_, current_head, 1);
      current_head = next_head;
    }
  }
};