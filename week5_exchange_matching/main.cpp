#include "order.hpp"
#include "exchange.hpp"
#include <memory>
#include <atomic>

std::atomic<uint64_t> timestamp_counter(1);

int main() {
    Exchange exchange;

    // AAPL
    auto a1 = std::make_shared<Order>("A1", "AAPL", true, 150.0, 10, timestamp_counter++);
    auto a2 = std::make_shared<Order>("A2", "AAPL", false, 149.0, 10, timestamp_counter++);
    auto a3 = std::make_shared<Order>("A3", "AAPL", true, 151.0, 5, timestamp_counter++);
    auto a4 = std::make_shared<Order>("A4", "AAPL", false, 150.0, 3, timestamp_counter++);
    auto a5 = std::make_shared<Order>("A5", "AAPL", false, 151.0, 4, timestamp_counter++);
    exchange.add_order(a1);
    exchange.add_order(a2);
    exchange.add_order(a3);
    exchange.add_order(a4);
    exchange.add_order(a5);

    // TSLA
    auto t1 = std::make_shared<Order>("T1", "TSLA", true, 700.0, 5, timestamp_counter++);
    auto t2 = std::make_shared<Order>("T2", "TSLA", false, 699.0, 5, timestamp_counter++);
    auto t3 = std::make_shared<Order>("T3", "TSLA", false, 702.0, 6, timestamp_counter++);
    auto t4 = std::make_shared<Order>("T4", "TSLA", true, 703.0, 3, timestamp_counter++);
    exchange.add_order(t1);
    exchange.add_order(t2);
    exchange.add_order(t3);
    exchange.add_order(t4);

    // GOOG
    auto g1 = std::make_shared<Order>("G1", "GOOG", true, 2800.0, 8, timestamp_counter++);
    auto g2 = std::make_shared<Order>("G2", "GOOG", false, 2795.0, 6, timestamp_counter++);
    exchange.add_order(g1);
    exchange.add_order(g2);

    // Cancel and Modify Test
    exchange.cancel_order("AAPL", "A1");
    exchange.modify_order("TSLA", "T1", 695.0, 5);
    exchange.cancel_order("GOOG", "G2");
    exchange.modify_order("GOOG", "G1", 2790.0, 10);

    // Print books
    exchange.print_order_book("AAPL");
    exchange.print_order_book("TSLA");
    exchange.print_order_book("GOOG");


    return 0;
}