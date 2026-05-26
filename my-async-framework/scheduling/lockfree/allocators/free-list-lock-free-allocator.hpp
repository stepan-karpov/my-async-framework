#pragma once

#include <cstdint>
#include <atomic>
#include <cassert>
#include <new>

// Lockfree algorithm
// Default values initialization
// Memory models
// Allocators vs owning memory
// ABA problem
// Use-After-Free problem
// Alignment

template <typename T>
concept BigEnoughToStorePointer = sizeof(T) >= sizeof(uintptr_t);

template <typename T>
requires BigEnoughToStorePointer<T>
class FreeListLockfreeAllocator {
 private:
  struct Slot {
    Slot* next_slot;
  };
 private:
  using TaggedPtr = uintptr_t;
  using TagType = uint16_t;

  inline static TaggedPtr Pack(Slot* ptr, TagType tag) {
    return reinterpret_cast<TaggedPtr>(ptr) |  (static_cast<TaggedPtr>(tag) << 48);
  }

  inline static Slot* UnpackPtr(TaggedPtr tagged_ptr) {
    return reinterpret_cast<Slot*>(tagged_ptr & 0x0000FFFFFFFFFFFF);
  }

  inline static TagType UnpackTag(TaggedPtr tagged_ptr) {
    return static_cast<TagType>(tagged_ptr >> 48);
  }
 private:
  void* memory_;
  std::atomic<TaggedPtr> head_;
 public:
  using value_type = T;
 public:
  FreeListLockfreeAllocator(const size_t capacity)
    : memory_(::operator new(capacity * sizeof(T), std::align_val_t(alignof(T))))
      , head_(Pack(reinterpret_cast<Slot*>(memory_), 0))
  {
    Slot* current_slot = UnpackPtr(head_.load(std::memory_order::relaxed));
    for (int i = 0; i < capacity - 1; ++i) {
      T* next_slot = reinterpret_cast<T*>(current_slot) + 1;
      Slot* next_slot_casted = reinterpret_cast<Slot*>(next_slot);
      current_slot->next_slot = next_slot_casted;
      
      current_slot = next_slot_casted;
    }

    current_slot->next_slot = nullptr;
  };

  FreeListLockfreeAllocator(const FreeListLockfreeAllocator&) = delete;
  FreeListLockfreeAllocator& operator=(const FreeListLockfreeAllocator&) = delete;

  FreeListLockfreeAllocator(FreeListLockfreeAllocator&& other) = delete;
  FreeListLockfreeAllocator& operator=(FreeListLockfreeAllocator&& other) = delete;

  ~FreeListLockfreeAllocator() {
    if (!memory_) { return; }
    ::operator delete(memory_, std::align_val_t(alignof(T)));
  }

  T* allocate(size_t n) {
    assert(n == 1); // only in debug/tests, hide under a macros

    while (true) {
      TaggedPtr current_head_tagged = head_.load(std::memory_order::acquire);
      
      Slot* current_head = UnpackPtr(current_head_tagged);
      TagType new_tag = UnpackTag(current_head_tagged) + 1;

      if (current_head == nullptr) {
        return nullptr;
      }

      Slot* next_head = current_head->next_slot;

      if (head_.compare_exchange_weak(
          current_head_tagged,
          Pack(next_head,new_tag),
          std::memory_order::acquire,
          std::memory_order::relaxed
      )) {
        return reinterpret_cast<T*>(current_head);
      }
    }
  }

  void deallocate(T* ptr, size_t n) {
    assert(n == 1); // only in debug/tests, hide under a macros

    Slot* current_slot = reinterpret_cast<Slot*>(ptr);

    while (true) {
      TaggedPtr current_head_tagged = head_.load(std::memory_order::relaxed);
      Slot* current_head = UnpackPtr(current_head_tagged);
      TagType new_tag = UnpackTag(current_head_tagged) + 1;

      current_slot->next_slot = current_head;

      if (head_.compare_exchange_weak(
          current_head_tagged,
          Pack(current_slot, new_tag),
          std::memory_order::release,
          std::memory_order::relaxed
        )) {
        return;
      }
    }
  }

  // void construct(T* ptr); // typical, use allocator_traits
  // void destroy(T* ptr); // typical, use allocator_traits
  // struct rebind {}; // typical, use allocator_traits  
};