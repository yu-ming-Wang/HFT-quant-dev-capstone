#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <unordered_map>
#include <atomic>
#include <random>
#include <chrono>
#include <set>
#include "thread_safe_queue.hpp"

struct Order {
    int id;
    std::string symbol;
    double price;
    int quantity;
    std::chrono::steady_clock::time_point enqueue_time;
};

std::atomic<int> global_order_id(1);
std::unordered_map<int, Order> pushed_orders; // For correctness check
std::mutex pushed_mutex;
std::mutex print_mutex; // global print lock

void orderbook_worker(
    const std::string& symbol,
    ThreadSafeQueue<Order>& queue,
    std::atomic<bool>& running,
    std::set<int>& matched_orders,
    std::mutex& matched_mutex,
    std::vector<double>& latencies
) {
    while (running) {
        Order order;
        if (queue.pop(order)) {
            if (order.id < 0) break;
            auto match_time = std::chrono::steady_clock::now();
            double latency_ms = std::chrono::duration<double, std::milli>(match_time - order.enqueue_time).count();
            latencies.push_back(latency_ms);

            // 記錄已撮合 order id
            {
                std::lock_guard<std::mutex> lock(matched_mutex);
                matched_orders.insert(order.id);
            }
            // 同步輸出，防止交錯
            {
                std::lock_guard<std::mutex> lock(print_mutex);
                std::cout << "[MATCH] Symbol: " << symbol << ", OrderID: " << order.id
                          << ", Latency: " << latency_ms << " ms\n";
            }
        }
    }
}

int main() {
    std::vector<std::string> symbols = {"AAPL", "GOOG", "MSFT"};
    std::unordered_map<std::string, ThreadSafeQueue<Order>> queues;
    std::unordered_map<std::string, std::atomic<bool>> running_flags;
    std::vector<std::thread> workers;
    std::unordered_map<std::string, std::set<int>> matched_orders;
    std::unordered_map<std::string, std::mutex> matched_mutexes;
    std::unordered_map<std::string, std::vector<double>> latencies;

    // 1. 啟動每個 symbol 的撮合 worker
    for (const auto& symbol : symbols) {
        running_flags[symbol] = true;
        workers.emplace_back(orderbook_worker, symbol,
            std::ref(queues[symbol]), std::ref(running_flags[symbol]),
            std::ref(matched_orders[symbol]), std::ref(matched_mutexes[symbol]),
            std::ref(latencies[symbol]));
    }

    // 2. 多執行緒產生訂單
    int num_threads = 4, orders_per_thread = 5000;
    std::vector<std::thread> producers;

    auto producer_func = [&](int tid) {
        std::mt19937 rng(tid + time(0));
        std::uniform_int_distribution<int> dist_symbol(0, symbols.size() - 1);
        std::uniform_real_distribution<double> dist_price(100, 200);
        std::uniform_int_distribution<int> dist_qty(1, 10);

        for (int i = 0; i < orders_per_thread; ++i) {
            std::string symbol = symbols[dist_symbol(rng)];
            int id = global_order_id++;
            Order order{id, symbol, dist_price(rng), dist_qty(rng), std::chrono::steady_clock::now()};
            {
                std::lock_guard<std::mutex> lock(pushed_mutex);
                pushed_orders[id] = order;
            }
            queues[symbol].push(order);
        }
    };
    auto t_start = std::chrono::steady_clock::now();
    for (int t = 0; t < num_threads; ++t) {
        producers.emplace_back(producer_func, t);
    }
    for (auto& t : producers) t.join();

    // 3. 等所有訂單消化完畢
    std::this_thread::sleep_for(std::chrono::seconds(3));
    for (auto& [symbol, flag] : running_flags) flag = false;
    for (auto& q : queues) q.second.push(Order{-1, "", 0, 0, std::chrono::steady_clock::now()}); // 結束
    for (auto& t : workers) t.join();

    auto t_end = std::chrono::steady_clock::now();
    double total_ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    int total_orders = num_threads * orders_per_thread;
    double throughput = 1000.0 * total_orders / total_ms;

    // 4. 正確性驗證
    int matched_total = 0;
    for (const auto& symbol : symbols) {
        matched_total += matched_orders[symbol].size();
    }
    {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "\n[RESULT] Total pushed: " << total_orders << ", matched: " << matched_total << std::endl;
        if (matched_total == total_orders) std::cout << "✅ All orders matched!\n";
        else std::cout << "❌ Order lost or duplicated!\n";
    }

    // 5. Latency 統計
    std::vector<double> all_lat;
    for (const auto& symbol : symbols)
        all_lat.insert(all_lat.end(), latencies[symbol].begin(), latencies[symbol].end());
    double avg_latency = 0.0;
    if (!all_lat.empty()) {
        for (double l : all_lat) avg_latency += l;
        avg_latency /= all_lat.size();
    }
    {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "[STATS] Throughput: " << throughput << " orders/sec, Avg Latency: " << avg_latency << " ms\n";
    }
    return 0;
}
