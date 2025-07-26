#pragma once
#include "order.hpp"
#include <deque>
#include <vector>
#include <map>
#include <string>
#include <iostream>

class OrderBook {
public:
    OrderBook(const std::string& symbol) : symbol_(symbol) {}

    std::vector<Order> add_order(const Order& order) {
        std::vector<Order> fills;
        if (order.type == OrderType::MARKET) {
            if (order.is_buy) {
                // 市價買單：撮合賣方
                auto it = sell_book_.begin();
                int qty_left = order.quantity;
                while (it != sell_book_.end() && qty_left > 0) {
                    auto& queue = it->second;
                    while (!queue.empty() && qty_left > 0) {
                        Order& ask = queue.front();
                        int match_qty = std::min(qty_left, ask.quantity);

                        Order fill = order;
                        fill.price = ask.price;
                        fill.quantity = match_qty;
                        fills.push_back(fill);

                        ask.quantity -= match_qty;
                        qty_left -= match_qty;

                        if (ask.quantity == 0) queue.pop_front();
                    }
                    if (queue.empty()) it = sell_book_.erase(it);
                    else ++it;
                }
            } else {
                // 市價賣單：撮合買方（reverse_iterator）
                auto it = buy_book_.rbegin();
                int qty_left = order.quantity;
                while (it != buy_book_.rend() && qty_left > 0) {
                    auto& queue = it->second;
                    while (!queue.empty() && qty_left > 0) {
                        Order& bid = queue.front();
                        int match_qty = std::min(qty_left, bid.quantity);

                        Order fill = order;
                        fill.price = bid.price;
                        fill.quantity = match_qty;
                        fills.push_back(fill);

                        bid.quantity -= match_qty;
                        qty_left -= match_qty;

                        if (bid.quantity == 0) queue.pop_front();
                    }
                    if (queue.empty()) {
                        // erase trick for reverse_iterator
                        auto base_it = std::next(it).base(); // convert to forward iterator
                        it = std::make_reverse_iterator(buy_book_.erase(base_it));
                    } else {
                        ++it;
                    }
                }
            }
        } else {
            // 限價單
            if (order.is_buy) {
                int qty_left = order.quantity;
                auto it = sell_book_.begin();
                while (it != sell_book_.end() && it->first <= order.price && qty_left > 0) {
                    auto& queue = it->second;
                    while (!queue.empty() && qty_left > 0) {
                        Order& ask = queue.front();
                        int match_qty = std::min(qty_left, ask.quantity);

                        Order fill = order;
                        fill.price = ask.price;
                        fill.quantity = match_qty;
                        fills.push_back(fill);

                        ask.quantity -= match_qty;
                        qty_left -= match_qty;

                        if (ask.quantity == 0) queue.pop_front();
                    }
                    if (queue.empty()) it = sell_book_.erase(it);
                    else ++it;
                }
                if (qty_left > 0) {
                    Order remain = order;
                    remain.quantity = qty_left;
                    buy_book_[order.price].push_back(remain);
                }
            } else {
                int qty_left = order.quantity;
                auto it = buy_book_.rbegin();
                while (it != buy_book_.rend() && it->first >= order.price && qty_left > 0) {
                    auto& queue = it->second;
                    while (!queue.empty() && qty_left > 0) {
                        Order& bid = queue.front();
                        int match_qty = std::min(qty_left, bid.quantity);

                        Order fill = order;
                        fill.price = bid.price;
                        fill.quantity = match_qty;
                        fills.push_back(fill);

                        bid.quantity -= match_qty;
                        qty_left -= match_qty;

                        if (bid.quantity == 0) queue.pop_front();
                    }
                    if (queue.empty()) {
                        auto base_it = std::next(it).base();
                        it = std::make_reverse_iterator(buy_book_.erase(base_it));
                    } else {
                        ++it;
                    }
                }
                if (qty_left > 0) {
                    Order remain = order;
                    remain.quantity = qty_left;
                    sell_book_[order.price].push_back(remain);
                }
            }
        }
        return fills;
    }

    void print_book(int depth = 5) const {
        std::cout << "OrderBook [" << symbol_ << "]\n";
        std::cout << " Asks (Sell):\n";
        int ac = 0;
        for (auto it = sell_book_.begin(); it != sell_book_.end() && ac < depth; ++it, ++ac) {
            int qty = 0; for (const auto& o : it->second) qty += o.quantity;
            std::cout << "   " << it->first << " x " << qty << '\n';
        }
        std::cout << " Bids (Buy):\n";
        int bc = 0;
        for (auto it = buy_book_.rbegin(); it != buy_book_.rend() && bc < depth; ++it, ++bc) {
            int qty = 0; for (const auto& o : it->second) qty += o.quantity;
            std::cout << "   " << it->first << " x " << qty << '\n';
        }
        std::cout << "====================\n";
    }

private:
    std::string symbol_;
    std::map<double, std::deque<Order>> buy_book_;  // buy：價高優先
    std::map<double, std::deque<Order>> sell_book_; // sell: 價低優先
};
