#pragma once

#include <atomic>
#include <memory>
#include <cassert>
#include <vector>

template<typename T>
class MPSCUnboundedQueue {
 private:
  struct BaseNode {
    std::atomic<BaseNode*> next_value = nullptr;
    virtual ~BaseNode() = default;
  };
  struct Node : BaseNode {
    T value;
  };
 private:
  alignas(64) std::atomic<BaseNode*> tail_ = nullptr;
  alignas(64) BaseNode* head_ = nullptr;
 public:
  MPSCUnboundedQueue()
  {
    BaseNode* dummy_node = new BaseNode(nullptr);
    head_ = dummy_node;
    tail_.store(dummy_node, std::memory_order::relaxed);
  }

  void Push(T value) {
    // add allocator
    BaseNode* new_tail = new Node{nullptr, std::move(value)};

    BaseNode* prev = tail_.exchange(new_tail, std::memory_order::acq_rel);
    prev->next_value.store(new_tail, std::memory_order::release);
  }

  bool Pop(T& value) {
    BaseNode* dummy_head = head_;
    Node* next = static_cast<Node*>(dummy_head->next_value.load(std::memory_order::acquire));

    if (next == nullptr) [[ unlikely ]] {
      return false;
    }

    value = std::move(next->value);
    head_ = next;
    delete dummy_head;

    return true;
  }

  ~MPSCUnboundedQueue() {
    while (head_ != nullptr) {
      Node* next = static_cast<Node*>(head_->next_value.load(std::memory_order::relaxed));
      if (next == nullptr) [[ unlikely ]] {
        break;
      }
      delete head_;
      head_ = next;
    }
    delete head_;
  }
};


template <typename T, typename Allocator = std::allocator<T>>
class MPSCUnboundedLazyQueue {
 private:
  struct Node {
    T value;
    Node* last_value_ptr = nullptr;
  };
 private:
  using AllocTTraits = std::allocator_traits<Allocator>;
  using AllocatorNode = AllocTTraits::template rebind_alloc<Node>;
  using AllocNodeTraits = std::allocator_traits<AllocatorNode>;
 private:
  [[ no_unique_address ]] AllocatorNode allocator_ = AllocatorNode();
  alignas(64) std::atomic<Node*> head_ = nullptr;
 public:
  MPSCUnboundedLazyQueue(Allocator allocator = Allocator())
    : allocator_(allocator)
      , head_(nullptr)
  {}

  void Push(T object) {
    Node* new_head = AllocNodeTraits::allocate(allocator_, 1);
    AllocNodeTraits::construct(allocator_, new_head, std::move(object));

    while (true) {
      Node* current_head = head_.load(std::memory_order::relaxed);
      new_head->last_value_ptr = current_head;

      if (head_.compare_exchange_weak(
          current_head,
          new_head,
          std::memory_order::release,
          std::memory_order::relaxed
      )) {
        return;
      }
    }
  }

  std::vector<T> Pop() {
    Node* last_head = head_.exchange(nullptr, std::memory_order::acquire);
    
    std::vector<T> objects; // TODO: make intrusive
    while (last_head != nullptr) {
      objects.push_back(std::move(last_head->value));
      auto last_head_copy = last_head;
      last_head = last_head->last_value_ptr;
      AllocNodeTraits::destroy(allocator_, last_head_copy);
      AllocNodeTraits::deallocate(allocator_, last_head_copy, 1);
    }

    return objects;
  }

  ~MPSCUnboundedLazyQueue() {
    Node* current_head = head_.load();
    while (current_head != nullptr) {
      auto current_head_copy = current_head;
      current_head = current_head->last_value_ptr;
      AllocNodeTraits::destroy(allocator_, current_head_copy);
      AllocNodeTraits::deallocate(allocator_, current_head_copy, 1);
    }
  }
};