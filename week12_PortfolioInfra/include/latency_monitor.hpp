#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <fstream>

class LatencyMonitor {
public:
    void record(long ns);
    void record_symbol(const std::string& symbol, long ns);

    void export_csv(const std::string& filename) const;
    void export_symbol_csv(const std::string& filename) const;

private:
    mutable std::mutex mtx;
    std::vector<long> samples;   // total latency
    std::unordered_map<std::string, std::vector<long>> symbol_samples; // every symbol latency
};
