#pragma once
#include <unordered_map>
#include <mutex>
#include <vector>
#include <string>

class PortfolioManager {
public:
    void record_trade(int order_id, double pnl);
    void report() const;
    void export_csv(const std::string& filename) const;


private:
    std::unordered_map<int, double> pnl_map;
    mutable std::mutex mtx;
};
