#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <unordered_map>
#include <atomic>
#include <random>
#include "thread_safe_queue.hpp"

struct Order {
    int id;
    std::string symbol;
    double price;
    int quantity;
};

std::atomic<int> global_order_id(1);

// Worker for each symbol
//std::atomic<bool>& running 是用來 thread-safe 地通知 worker thread 什麼時候結束工作。
//它保證跨 thread 訊號安全傳遞，讓我能正確地終止多個 worker thread。
void orderbook_worker(const std::string& symbol, ThreadSafeQueue<Order>& queue, std::atomic<bool>& running) {
    while (running) {
        Order order;
        if (queue.pop(order)) {
            // 模擬撮合
            std::cout << "[MATCH] Symbol: " << symbol << ", OrderID: " << order.id 
                      << ", Price: " << order.price << ", Qty: " << order.quantity << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 模擬撮合延遲
        }
    }
}


int main() {
    std::vector<std::string> symbols = {"AAPL", "GOOG", "MSFT"};
    std::unordered_map<std::string, ThreadSafeQueue<Order>> queues;
    std::unordered_map<std::string, std::atomic<bool>> running_flags;
    std::vector<std::thread> workers;

    // 啟動每個 symbol 的撮合 worker
    for (const auto& symbol : symbols) {
        running_flags[symbol] = true;
        workers.emplace_back(orderbook_worker, symbol, std::ref(queues[symbol]), std::ref(running_flags[symbol]));
    }

    // 隨機產生訂單分派給各 symbol
    std::mt19937 rng((unsigned int)time(0));
    std::uniform_int_distribution<int> dist_symbol(0, symbols.size() - 1);
    std::uniform_real_distribution<double> dist_price(100, 200);
    std::uniform_int_distribution<int> dist_qty(1, 10);

    for (int i = 0; i < 30; ++i) {
        std::string symbol = symbols[dist_symbol(rng)];
        Order order{global_order_id++, symbol, dist_price(rng), dist_qty(rng)};
        queues[symbol].push(order);
        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // 模擬外部下單頻率
    }

    // 等待剩餘訂單處理
    std::this_thread::sleep_for(std::chrono::seconds(2));
    for (auto& [symbol, flag] : running_flags) flag = false;
    for (auto& q : queues) {
        Order dummy{-1, "", 0, 0};
        q.second.push(dummy); // 通知 worker 結束
    }
    for (auto& t : workers) t.join();
    std::cout << "All matching threads stopped." << std::endl;
    return 0;
}
