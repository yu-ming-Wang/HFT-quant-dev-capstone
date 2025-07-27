#include <iostream>
#include <vector>
#include <string>
#include <atomic>
#include <algorithm>
#include <fstream>   
#include "profiling.hpp"
#include "ring_buffer.hpp"
#include "fast_logger.hpp"
#include "orderbook_optimized.hpp"
#include "order.hpp"

// latency statistic
struct LatencyStats {
    std::vector<long> samples;
    void add(long ns) { samples.push_back(ns); }

    void report() {
        if (samples.empty()) return;
        std::sort(samples.begin(), samples.end());
        auto p = [&](double q){
            size_t idx = std::min<size_t>((size_t)(q * samples.size()), samples.size() - 1);
            return samples[idx];
        };
        std::cout << "[Latency] p50=" << p(0.5)
                  << "ns, p90=" << p(0.9)
                  << "ns, p99=" << p(0.99)
                  << "ns, p99.9=" << p(0.999) << "ns\n";
    }

    //  write to CSV
    void dump_csv(const std::string& filename) {
        std::ofstream file(filename);
        file << "latency_ns\n";
        for (auto ns : samples) {
            file << ns << "\n";
        }
        file.close();
        std::cout << "[CSV] Latency samples saved to " << filename << "\n";
    }
};

int main() {
    OrderBookOptimized book;
    LockFreeRingBuffer<Order, 4096> queue;
    FastLogger logger("week11_trades.log");
    Profiler profiler;
    LatencyStats stats;

    // test order
    for (int i = 0; i < 10000; i++) {
        queue.push(Order(i, "AAPL", i % 2, 150.0 + (i % 5), 10, OrderType::LIMIT));
    }

    // match
    while (auto o = queue.pop()) {
        profiler.start();
        book.add_order(*o);
        auto fills = book.match(*o);
        long ns = profiler.stop("Match order");
        stats.add(ns);

        for (auto& f : fills) {
            logger.log("TRADE " + f.symbol + " @ " + std::to_string(f.price));
        }
    }

    stats.report();
    stats.dump_csv("week11_latency.csv");   // ✅ 新增 CSV 輸出
    std::cout << "[DONE] Week11 Low Latency Test Finished\n";
    return 0;
}
