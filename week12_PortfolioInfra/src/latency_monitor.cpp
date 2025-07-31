#include "latency_monitor.hpp"
#include <algorithm>
#include <iostream>

void LatencyMonitor::record(long ns) {
    std::lock_guard<std::mutex> lock(mtx);
    samples.push_back(ns);
}

void LatencyMonitor::record_symbol(const std::string& symbol, long ns) {
    std::lock_guard<std::mutex> lock(mtx);
    symbol_samples[symbol].push_back(ns);
}

void LatencyMonitor::export_csv(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(mtx);
    std::ofstream out(filename);
    out << "latency_ns\n";
    for (auto ns : samples) out << ns << "\n";
}

void LatencyMonitor::export_symbol_csv(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(mtx);
    std::ofstream out(filename);
    out << "symbol,latency_ns\n";
    for (const auto& [sym, vec] : symbol_samples) {
        for (auto ns : vec) {
            out << sym << "," << ns << "\n";
        }
    }
}
