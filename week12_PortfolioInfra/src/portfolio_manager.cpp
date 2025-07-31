#include "portfolio_manager.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

void PortfolioManager::record_trade(int order_id, double pnl) {
    std::lock_guard<std::mutex> lock(mtx);
    pnl_map[order_id] += pnl;
}

void PortfolioManager::export_csv(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(mtx);
    std::ofstream out(filename);
    out << "order_id,pnl\n";   // ✅ header
    for (const auto& [order_id, pnl] : pnl_map) {
        out << order_id << "," << pnl << "\n";
    }
}

void PortfolioManager::report() const {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "=== Portfolio PnL ===\n";
    for (const auto& [order_id, pnl] : pnl_map) {
        std::cout << "Order " << order_id << " : " << pnl << "\n";
    }
}
