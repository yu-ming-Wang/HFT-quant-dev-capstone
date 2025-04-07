#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue {
public:
    //RAII(Resource Acquisition is Initialization)
    void push(const T& item) {
        std::lock_guard<std::mutex> lock(m_);   //lock
        q_.push(item);
        cv_.notify_one();     //wake consumer thread

        //when it leave Scope auto unlock 
    }

    T pop() {
        std::unique_lock<std::mutex> lock(m_);
        
        //lambda expression
        cv_.wait(lock, [this]{ return !q_.empty(); });  //while queue is empty, wait
        //when waiting queue have data, mutex unlock 
        
        // while(q_.empty()){
        //     cv_.wait(lock);       
        // }

        T item = q_.front();
        q_.pop();
        return item;
    }

private:
    std::queue<T> q_;
    std::mutex m_;
    std::condition_variable cv_;  //let thread wait for queue have data to 
};
