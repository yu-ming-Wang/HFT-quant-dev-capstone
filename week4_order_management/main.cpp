#include "order.hpp"
#include "order_book.hpp"
#include "thread_safe_queue.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <memory>

std::atomic<uint64_t> timestamp_counter(1);

int main() {
    OrderBook book;

    auto o1 = std::make_shared<Order>("ORD1", true, 101.0, 10, timestamp_counter++);
    auto o2 = std::make_shared<Order>("ORD2", false, 100.0, 10, timestamp_counter++);
    auto o3 = std::make_shared<Order>("ORD3", true, 100.5, 5, timestamp_counter++);
    auto o4 = std::make_shared<Order>("ORD4", false, 99.5, 8, timestamp_counter++);
    auto o5 = std::make_shared<Order>("ORD5", true, 102.0, 6, timestamp_counter++);
    auto o6 = std::make_shared<Order>("ORD6", false, 103.0, 5, timestamp_counter++);


    book.add_order(o1);
    book.add_order(o2);
    book.add_order(o3);
    book.add_order(o4);
    book.add_order(o5);
    book.add_order(o6);


    book.cancel_order("ORD3");
    book.modify_order("ORD1", 99.0, 8);
    book.modify_order("ORD2", 98.5, 5);
    book.cancel_order("ORD5");
    book.modify_order("ORD6", 101.5, 5); 

    return 0;
}