#pragma once
#include <atomic>
#include <optional>
#include <cstddef>

template<typename T, size_t SIZE>
class LockFreeRingBuffer {
public:
    LockFreeRingBuffer() : head(0), tail(0) {}

    bool push(const T& item) {
        size_t next = (head + 1) % SIZE;
        if (next == tail.load(std::memory_order_acquire)) return false; // full
        buffer[head] = item;
        head = next;
        return true;
    }

    std::optional<T> pop() {
        if (tail.load(std::memory_order_acquire) == head) return std::nullopt; // empty
        T item = buffer[tail];
        tail = (tail + 1) % SIZE;
        return item;
    }

    bool empty() const { return head == tail.load(); }
    bool full() const { return ((head + 1) % SIZE) == tail.load(); }

private:
    T buffer[SIZE];
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
};
