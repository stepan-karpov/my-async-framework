#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <optional>

// Lockfree algorithm
// Memory models
// Allocator
// ABA Problem
// Use-After-Free Problem

/*
If allocator is not free-list, then
use-after-free in Pop() can happen
*/
template <typename T, typename AllocatorTType = std::allocator<T>>
class UnboundedStack {
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
  using AllocTraitsT = std::allocator_traits<AllocatorTType>;
  using AllocatorNodeType = AllocTraitsT::template rebind_alloc<Node>;
  using AllocTraitsNode = std::allocator_traits<AllocatorNodeType>;
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
  alignas(64) std::atomic<TaggedPtr> head_{0x0000000000000000};
  alignas(64) [[ no_unique_address ]] AllocatorNodeType allocator_ = AllocatorNodeType();
 public:
  UnboundedStack(AllocatorTType allocator = AllocatorTType())
    : allocator_(allocator)
  {}

  void Push(T object) {
    Node* new_head = AllocTraitsNode::allocate(allocator_, 1);
    AllocTraitsNode::construct(allocator_, new_head, std::move(object));

    while (true) {
      TaggedPtr old_head_tagged = head_.load(std::memory_order::relaxed);
      Node* old_head = UnpackPtr(old_head_tagged);
      TagType new_tag = UnpackTag(old_head_tagged) + 1;

      new_head->last_value_ptr = old_head;

      if (head_.compare_exchange_weak(
          old_head_tagged,
          Pack(new_head, new_tag),
          std::memory_order::release,
          std::memory_order::relaxed
        )) {
        return;
      }
    }
  }

  std::optional<T> Pop() {
    Node* old_head;

    while (true) {
      TaggedPtr old_head_tagged = head_.load(std::memory_order::acquire);
      old_head = UnpackPtr(old_head_tagged); 
      TagType new_tag = UnpackTag(old_head_tagged) + 1; 

      if (old_head == nullptr) {
        return std::nullopt;
      }

      Node* new_head = old_head->last_value_ptr; // use-after-free with default allocator

      if (head_.compare_exchange_weak(
        old_head_tagged,
        Pack(new_head, new_tag),
        std::memory_order::acq_rel,
        std::memory_order::relaxed
      )) {
        break;
      }
    }

    auto value_to_return = std::move(old_head->value);
    AllocTraitsNode::destroy(allocator_, old_head);
    AllocTraitsNode::deallocate(allocator_, old_head, 1);
    return value_to_return;
  }

  ~UnboundedStack() {
    Node* current_head = UnpackPtr(head_.load(std::memory_order_relaxed));
    while (current_head != nullptr) {
      Node* next_head = current_head->last_value_ptr;
      AllocTraitsNode::destroy(allocator_, current_head);
      AllocTraitsNode::deallocate(allocator_, current_head, 1);
      current_head = next_head;
    }
  }
};