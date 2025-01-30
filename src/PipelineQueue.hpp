#ifndef PIPELINE_QUEUE_HPP
#define PIPELINE_QUEUE_HPP

#include <assert.h>
#include <atomic>
#include <thread>
#include <vector>

#include "SortOfLockFreeQueue.hpp"


template<typename T>
class PipelineQueue {
using Queue = SortOfLockFreeQueue<T>;

std::atomic<uint32_t> producersCount;
Queue queue;

public:
    PipelineQueue(size_t buffer_size) : queue(buffer_size) {};

    template <typename... Args> void enqueue(Args &&...args) {
        queue.enqueue(T(std::forward<Args>(args)...));
    }

    bool tryDequeue(T &data) {
        return queue.tryDequeue(data);
    }

    bool isDone() const {
        // we don't have a clear order
        return producersCount.load(std::memory_order_relaxed) == 0 && queue.isEmpty();
    }

    void done() {
        // we don't have a clear order
        assert(producersCount.load(std::memory_order_relaxed) != 0);
        producersCount.fetch_sub(1);
    }

    bool isEmpty() const { return queue.isEmpty(); }
};

#endif // PIPELINE_QUEUE_HPP