#ifndef SORT_OF_LOCK_FREE_QUEUE_HPP
#define SORT_OF_LOCK_FREE_QUEUE_HPP

#include <assert.h>
#include <atomic>
#include <thread>
#include <vector>

template <typename T> class SortOfLockFreeQueue {

  // using unsigned to allow legal wrap around
  struct CellTy {
    std::atomic<uint32_t> sequence;
    T data;
  };

  // make sure core values are in the different cache lines
  alignas(64) std::vector<CellTy> buffer;
  alignas(64) uint32_t buffer_mask;
  alignas(64) std::atomic<uint32_t> enqueue_pos, dequeue_pos;

public:
  SortOfLockFreeQueue(size_t buffer_size)
      : buffer(buffer_size), buffer_mask(buffer_size - 1) {
    assert((buffer_size & (buffer_size - 1)) == 0 &&
           "buffer size expected to be power of 2");
    for (size_t i = 0; i != buffer_size; ++i)
      buffer[i].sequence.store(i, std::memory_order_relaxed);

    // initialize starting points
    enqueue_pos.store(0, std::memory_order_relaxed);
    dequeue_pos.store(0, std::memory_order_relaxed);
  }

  bool tryEnqueue(T data) {
    CellTy *cell;
    uint32_t pos;
    bool res = false;

    while (!res) {
      // fetch the current position where to enqueue the item
      pos = enqueue_pos.load(std::memory_order_relaxed);
      cell = &buffer[pos & buffer_mask];
      auto seq = cell->sequence.load(std::memory_order_acquire);
      auto diff = static_cast<int32_t>(seq) - static_cast<int32_t>(pos);

      // queue is full we cannot enqueue and just return false
      if (diff < 0)
        return false;

      // If its sequence wasn't touched by other producers check if we can
      // increment the enqueue position
      if (diff == 0)
        res = enqueue_pos.compare_exchange_weak(pos, pos + 1,
                                                std::memory_order_relaxed);
    }

    // write the item we want to enqueue and bump sequence
    cell->data = std::move(data);
    cell->sequence.store(pos + 1, std::memory_order_release);
    return true;
  }

  void enqueue(T data) {
    while (!tryEnqueue(data)) {
      std::this_thread::yield();
    }
  }

  template <typename... Args> void enqueue(Args &&...args) {
    enqueue(T(std::forward<Args>(args)...));
  }

  bool tryDequeue(T &data) {
    CellTy *cell;
    uint32_t pos;
    bool res = false;

    while (!res) {
      // fetch the current position from where we can dequeue an item
      pos = dequeue_pos.load(std::memory_order_relaxed);
      cell = &buffer[pos & buffer_mask];
      auto Seq = cell->sequence.load(std::memory_order_acquire);
      auto diff = static_cast<int32_t>(Seq) - static_cast<int32_t>(pos + 1);

      // probably the queue is empty, then return false
      if (diff < 0)
        return false;

      // Check if its sequence was changed by a producer and wasn't changed by
      // other consumers and if we can increment the dequeue position
      if (diff == 0)
        res = dequeue_pos.compare_exchange_weak(pos, pos + 1,
                                                std::memory_order_relaxed);
    }

    // read the item and update for the next round of the buffer
    data = std::move(cell->data);
    cell->sequence.store(pos + buffer_mask + 1, std::memory_order_release);
    return true;
  }

  bool isEmpty() const { return enqueue_pos.load() == dequeue_pos.load(); }
};

#endif // SORT_OF_LOCK_FREE_QUEUE_HPP