#pragma once
#include "orderbook.hpp"
#include "order.hpp"
#include "strategy.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

class Exchange {
public:
    // 初始化時註冊所有商品（symbol）
    Exchange(const std::vector<std::string>& symbols) {
        for (const auto& sym : symbols)
            orderbooks_[sym] = std::make_unique<OrderBook>(sym);
    }

    // 註冊策略（會設好策略對 exchange 的連結）
    void register_strategy(Strategy* s) {
        strategies_.push_back(s);
        s->set_exchange(this);
    }

    // 策略送單時呼叫（支援多 symbol）
    void submit_order(const Order& o) {
        auto it = orderbooks_.find(o.symbol);
        if (it != orderbooks_.end()) {
            // 撮合結果回傳成交明細（可多筆，partial fill）
            auto fills = it->second->add_order(o);
            for (const auto& fill : fills) {
                // 撮合回報給所有策略（如要區分可自行擴充）
                for (auto* s : strategies_)
                    s->on_order_filled(fill);
            }
        }
    }

    // 播行情給所有策略（主程式 loop 每行都會調用）
    void broadcast_market_data(const MarketData& md) {
        for (auto* s : strategies_)
            s->on_market_data(md);
    }

    // 查詢各 symbol 的 orderbook 狀態
    OrderBook* get_orderbook(const std::string& sym) {
        auto it = orderbooks_.find(sym);
        return it != orderbooks_.end() ? it->second.get() : nullptr;
    }

    void print_all_books() const {
        for (const auto& [sym, ob] : orderbooks_)
            ob->print_book();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<OrderBook>> orderbooks_;
    std::vector<Strategy*> strategies_;
};
