#pragma once
#include "order.hpp"
#include "trade.hpp"
#include <vector>
#include <map>
#include <deque>
#include <algorithm>

class OrderBookOptimized {
public:
    void add_order(const Order& o) {
        if (o.is_buy) {
            buy_levels[o.price].push_back(o);
        } else {
            sell_levels[o.price].push_back(o);
        }
    }

    std::vector<Trade> match() {
        std::vector<Trade> trades;
        // 用正向 iterator，方便在移除 price level 時不出錯
        auto buy_it = buy_levels.begin();
        auto sell_it = sell_levels.begin();

        while (buy_it != buy_levels.end() && sell_it != sell_levels.end()) {
            double buy_price = buy_it->first;
            double sell_price = sell_it->first;
            // 買價必須 >= 賣價才能撮合
            if (buy_price < sell_price) break;

            auto& buy_queue = buy_it->second;
            auto& sell_queue = sell_it->second;

            while (!buy_queue.empty() && !sell_queue.empty()) {
                Order& buy = buy_queue.front();
                Order& sell = sell_queue.front();
                int qty = std::min(buy.quantity, sell.quantity);
                trades.push_back(Trade{
                    buy.order_id, sell.order_id, buy.symbol, sell_price, qty
                });
                buy.quantity -= qty;
                sell.quantity -= qty;
                if (buy.quantity == 0) buy_queue.pop_front();
                if (sell.quantity == 0) sell_queue.pop_front();
            }

            // 清空後移除 price level，且必須小心處理 iterator 失效
            // 先記下目前 iterator
            auto old_buy = buy_it++;
            auto old_sell = sell_it++;
            if (old_buy->second.empty()) buy_levels.erase(old_buy);
            if (old_sell->second.empty()) sell_levels.erase(old_sell);
        }
        return trades;
    }

private:

    std::map<double, std::deque<Order>, std::greater<double>> buy_levels;
    std::map<double, std::deque<Order>, std::less<double>> sell_levels;
};
