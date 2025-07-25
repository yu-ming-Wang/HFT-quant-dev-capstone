#pragma once
#include "order.hpp"
#include <map>
#include <deque>
#include <unordered_map>
#include <iostream>
#include <iomanip>

class OrderBook {
public:
    OrderBook(const std::string& symbol) : symbol_(symbol), order_id_counter_(1) {}

    int add_order(bool is_buy, double price, int qty, OrderType type) {
        int oid = order_id_counter_++;
        Order order{oid, symbol_, is_buy, price, qty, type};
        if (type == OrderType::LIMIT) {
            auto& book = is_buy ? bids_ : asks_;
            book[price].push_back(order);
            id_map_[oid] = {is_buy, price};
        } else {
            match_market_order(order);
        }
        return oid;
    }

    // 撤單
    bool cancel_order(int order_id) {
        if (!id_map_.count(order_id)) return false;
        auto [is_buy, price] = id_map_[order_id];
        auto& book = is_buy ? bids_ : asks_;
        auto it = book.find(price);
        if (it == book.end()) return false;
        auto& queue = it->second;
        for (auto qit = queue.begin(); qit != queue.end(); ++qit) {
            if (qit->order_id == order_id) {
                queue.erase(qit);
                if (queue.empty()) book.erase(it);
                id_map_.erase(order_id);
                std::cout << "[CANCEL] OrderID: " << order_id << std::endl;
                return true;
            }
        }
        return false;
    }

    // 列印 orderbook 快照
    void print_book(int depth = 5) const {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "=== ORDER BOOK (" << symbol_ << ") ===" << std::endl;
        std::cout << " Asks:" << std::endl;
        int ac = 0;
        for (auto it = asks_.begin(); it != asks_.end() && ac < depth; ++it, ++ac) {
            int qty = 0; for (const auto& o : it->second) qty += o.quantity;
            std::cout << "   " << it->first << " x " << qty << std::endl;
        }
        std::cout << " Bids:" << std::endl;
        int bc = 0;
        for (auto it = bids_.rbegin(); it != bids_.rend() && bc < depth; ++it, ++bc) {
            int qty = 0; for (const auto& o : it->second) qty += o.quantity;
            std::cout << "   " << it->first << " x " << qty << std::endl;
        }
        std::cout << "====================" << std::endl;
    }
    
    void match_market_order(Order& mo) {
        if (mo.is_buy) {
            // 市價買單，從賣方最便宜（asks_.begin()）開始撮合
            auto it = asks_.begin();
            while (mo.quantity > 0 && it != asks_.end()) {
                double price = it->first;
                auto& queue = it->second;
                while (mo.quantity > 0 && !queue.empty()) {
                    auto& limit_order = queue.front();
                    int matched = std::min(mo.quantity, limit_order.quantity);
                    std::cout << "[MATCH] Market Buy matched with Sell @" << price << " x " << matched << std::endl;
                    mo.quantity -= matched;
                    limit_order.quantity -= matched;
                    if (limit_order.quantity == 0) {
                        id_map_.erase(limit_order.order_id);
                        queue.pop_front();
                    }
                }
                if (queue.empty()) it = asks_.erase(it);
                else ++it;
            }
        } else {
            // 市價賣單，從買方最貴（bids_.rbegin()）開始撮合
            auto it = bids_.rbegin();
            while (mo.quantity > 0 && it != bids_.rend()) {
                double price = it->first;
                auto& queue = it->second;
                while (mo.quantity > 0 && !queue.empty()) {
                    auto& limit_order = queue.front();
                    int matched = std::min(mo.quantity, limit_order.quantity);
                    std::cout << "[MATCH] Market Sell matched with Buy @" << price << " x " << matched << std::endl;
                    mo.quantity -= matched;
                    limit_order.quantity -= matched;
                    if (limit_order.quantity == 0) {
                        id_map_.erase(limit_order.order_id);
                        queue.pop_front();
                    }
                }
                // reverse_iterator 沒有 erase，只能用 tricky 方法
                if (queue.empty()) {
                    // 取得正向 iterator 並刪除
                    auto erase_it = std::next(it).base();
                    erase_it = bids_.erase(erase_it);
                    it = std::make_reverse_iterator(erase_it);
                } else {
                    ++it;
                }
            }
        }
        if (mo.quantity > 0)
            std::cout << "[UNFILLED] Market order partial/unfilled, qty left: " << mo.quantity << std::endl;
    }
    
    

private:
    std::string symbol_;
    int order_id_counter_;
    std::map<double, std::deque<Order>> bids_; // 降冪排列，買方
    std::map<double, std::deque<Order>> asks_; // 升冪排列，賣方
    std::unordered_map<int, std::pair<bool, double>> id_map_; // order_id -> (is_buy, price)
};
