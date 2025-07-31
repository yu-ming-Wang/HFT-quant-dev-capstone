#include "engine_manager.hpp"
#include "trade_logger.hpp"
#include <iostream>


void EngineManager::set_portfolio_manager(PortfolioManager* pm) {
    portfolio_mgr = pm;
}

void EngineManager::add_symbol(const std::string& symbol) {
    std::lock_guard<std::mutex> lock(mtx);
    books[symbol] = OrderBookOptimized();
}

void EngineManager::submit_order(const Order& order) {
    std::lock_guard<std::mutex> lock(mtx);
    books[order.symbol].add_order(order);
}

std::vector<Trade> EngineManager::match_all(LatencyMonitor* lm) {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<Trade> all_trades;
    for (auto& [symbol, book] : books) {
        
        //record latency for each book
        auto t0s = std::chrono::high_resolution_clock::now();
        auto trades = book.match();
        auto t1s = std::chrono::high_resolution_clock::now();
        long ns_sym = std::chrono::duration_cast<std::chrono::nanoseconds>(t1s - t0s).count();

        if (lm) lm->record_symbol(symbol, ns_sym);  // record per symbol latency

        if (portfolio_mgr) {
            for (const auto& t : trades) {
                // PnL = (買單) 買入成交數量 * 價格為負值，賣單為正值
                portfolio_mgr->record_trade(t.buy_order_id, -t.price * t.quantity);
                portfolio_mgr->record_trade(t.sell_order_id, t.price * t.quantity);
            }
        }
        all_trades.insert(all_trades.end(), trades.begin(), trades.end());
    }
    return all_trades;
}
