#pragma once
#include <unordered_map>
#include <string>
#include <mutex>
#include "orderbook_optimized.hpp"
#include "latency_monitor.hpp"
#include "order.hpp"
#include "trade.hpp"
#include "portfolio_manager.hpp"
#include "trade_logger.hpp"

class EngineManager {
public:
    void set_portfolio_manager(PortfolioManager* pm);
    //add symbol to orderbook map
    void add_symbol(const std::string& symbol);
    void submit_order(const Order& order);
    std::vector<Trade> match_all(LatencyMonitor* lm = nullptr);

private:
    //orderbook map for multi stock
    std::unordered_map<std::string, OrderBookOptimized> books;
    PortfolioManager* portfolio_mgr = nullptr;
    std::mutex mtx;
};
