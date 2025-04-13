#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue {
public:
    void push(const T& item) {
        std::lock_guard<std::mutex> lock(m_);
        q_.push(item);
        cv_.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(m_);
        cv_.wait(lock, [this]{ return !q_.empty(); });
        T item = q_.front();
        q_.pop();
        return item;
    }

private:
    std::queue<T> q_;
    std::mutex m_;
    std::condition_variable cv_;
};