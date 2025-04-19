// ===== order_book.hpp =====
#pragma once
#include "order.hpp"
#include <queue>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <memory>

class OrderBook {
public:
    struct TradeLogEntry {
        std::string buy_id;
        std::string sell_id;
        double price;
        int quantity;
    };

    void add_order(std::shared_ptr<Order> order) {
        order_map[order->id] = order;
        if (order->is_buy) {
            buy_orders.push(order);
        } else {
            sell_orders.push(order);
        }
        match_orders();
    }

    bool cancel_order(const std::string& order_id) {
        auto it = order_map.find(order_id);
        if (it != order_map.end()) {
            it->second->canceled = true;
            std::cout << "[CANCEL] Order " << order_id << " marked as canceled.\n";
            return true;
        }
        return false;
    }

    bool modify_order(const std::string& order_id, double new_price, int new_qty) {
        auto it = order_map.find(order_id);
        if (it != order_map.end()) {
            auto old_order = it->second;
            if (old_order->quantity == 0 || old_order->canceled) {
                std::cout << "[MODIFY-FAIL] Order " << order_id << " already filled or canceled.\n";
                return false;
            }

            old_order->canceled = true;
            std::cout << "[MODIFY] Canceling and replacing Order " << order_id << "\n";

            auto new_order = std::make_shared<Order>(order_id, old_order->symbol, old_order->is_buy, new_price, new_qty, old_order->timestamp);
            add_order(new_order);
            return true;
        }
        return false;
    }

    void print() const {
        std::priority_queue<std::shared_ptr<Order>, std::vector<std::shared_ptr<Order>>, BuyComp> buy_copy = buy_orders;
        std::priority_queue<std::shared_ptr<Order>, std::vector<std::shared_ptr<Order>>, SellComp> sell_copy = sell_orders;

        std::cout << "Buy Orders:\n";
        while (!buy_copy.empty()) {
            auto o = buy_copy.top(); buy_copy.pop();
            if (!o->canceled && o->quantity > 0)
                std::cout << "  [BUY]  " << o->id << " @" << o->price << " x " << o->quantity << "\n";
        }

        std::cout << "Sell Orders:\n";
        while (!sell_copy.empty()) {
            auto o = sell_copy.top(); sell_copy.pop();
            if (!o->canceled && o->quantity > 0)
                std::cout << "  [SELL] " << o->id << " @" << o->price << " x " << o->quantity << "\n";
        }

        std::cout << "Trade Log:\n";
        for (const auto& trade : trade_log) {
            std::cout << "  [TRADE] " << trade.buy_id << " x " << trade.sell_id
                      << " @" << trade.price << " x " << trade.quantity << "\n";
        }
    }

private:
    struct BuyComp {
        bool operator()(const std::shared_ptr<Order>& a, const std::shared_ptr<Order>& b) const {
            return a->price < b->price || (a->price == b->price && a->timestamp > b->timestamp);
        }
    };

    struct SellComp {
        bool operator()(const std::shared_ptr<Order>& a, const std::shared_ptr<Order>& b) const {
            return a->price > b->price || (a->price == b->price && a->timestamp > b->timestamp);
        }
    };

    std::priority_queue<std::shared_ptr<Order>, std::vector<std::shared_ptr<Order>>, BuyComp> buy_orders;
    std::priority_queue<std::shared_ptr<Order>, std::vector<std::shared_ptr<Order>>, SellComp> sell_orders;
    std::unordered_map<std::string, std::shared_ptr<Order>> order_map;
    std::vector<TradeLogEntry> trade_log;

    void match_orders() {
        while (!buy_orders.empty() && !sell_orders.empty()) {
            auto buy = buy_orders.top();
            auto sell = sell_orders.top();

            if (buy->canceled) { buy_orders.pop(); continue; }
            if (sell->canceled) { sell_orders.pop(); continue; }

            if (buy->price >= sell->price) {
                int matched_qty = std::min(buy->quantity, sell->quantity);
                std::cout << "[MATCH] " << buy->id << " x " << sell->id
                          << " @ " << sell->price << " x " << matched_qty << std::endl;

                trade_log.push_back({buy->id, sell->id, sell->price, matched_qty});

                buy->quantity -= matched_qty;
                sell->quantity -= matched_qty;

                if (buy->quantity == 0) buy_orders.pop();
                if (sell->quantity == 0) sell_orders.pop();
            } else {
                break;
            }
        }
    }
};
