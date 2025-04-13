#include "order.hpp"
#include "order_book.hpp"
#include "thread_safe_queue.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>

std::mutex print_mutex;
std::atomic<uint64_t> timestamp_counter(1);

void producer(ThreadSafeQueue<Order>& queue) {
    for (int i = 0; i < 5; ++i) {
        Order o("BUY" + std::to_string(i), true, 101.0 - i * 0.5, 10, timestamp_counter++);
        queue.push(o);
        {
            std::lock_guard<std::mutex> lock(print_mutex);
            std::cout << "[Producer] " << o.id << " submitted.\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    for (int i = 0; i < 5; ++i) {
        Order o("SELL" + std::to_string(i), false, 100.0 + i * 0.5, 10, timestamp_counter++);
        queue.push(o);
        {
            std::lock_guard<std::mutex> lock(print_mutex);
            std::cout << "[Producer] " << o.id << " submitted.\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void consumer(ThreadSafeQueue<Order>& queue, OrderBook& book, int count) {
    for (int i = 0; i < count; ++i) {
        Order o = queue.pop();
        {
            std::lock_guard<std::mutex> lock(print_mutex);
            std::cout << "[Consumer] Processing " << o.id << "\n";
        }
        book.add_order(o);
    }
}

int main() {
    ThreadSafeQueue<Order> queue;
    OrderBook book;

    std::thread p(producer, std::ref(queue));
    std::thread c(consumer, std::ref(queue), std::ref(book), 10);

    p.join();
    c.join();

    return 0;
}
