#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>

class ThreadSafeQueue {
public:
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(m_);
        q_.push(value);
        cv_.notify_one();
    }

    bool pop(T& value) {
        std::unique_lock<std::mutex> lock(m_);
        cv_.wait(lock, [this]{ return !q_.empty(); });
        value = q_.front();
        q_.pop();
        return true;
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(m_);
        return q_.empty();
    }

private:
    std::queue<T> q_;
    std::mutex m_;
    std::condition_variable cv_;
};
