#pragma once
#include "order.hpp"
#include <queue>
#include <vector>
#include <iostream>
#include <functional>

class OrderBook {
public:
    void add_order(const Order& order) {
        if (order.is_buy) {
            buy_orders.push(order);
        } else {
            sell_orders.push(order);
        }
        match_orders();
    }

private:
    struct BuyComp {
        bool operator()(const Order& a, const Order& b) {
            return a.price < b.price; // Max-heap
        }
    };
    struct SellComp {
        bool operator()(const Order& a, const Order& b) {
            return a.price > b.price; // Min-heap
        }
    };

    std::priority_queue<Order, std::vector<Order>, BuyComp> buy_orders;
    std::priority_queue<Order, std::vector<Order>, SellComp> sell_orders;

    void match_orders() {
        while (!buy_orders.empty() && !sell_orders.empty()) {
            Order buy = buy_orders.top();
            Order sell = sell_orders.top();

            if (buy.price >= sell.price) {
                int matched_qty = std::min(buy.quantity, sell.quantity);
                std::cout << "[MATCH] " << buy.id << " x " << sell.id
                          << " @ " << sell.price << " x " << matched_qty << std::endl;

                buy_orders.pop();
                sell_orders.pop();

                if (buy.quantity > matched_qty)
                    buy_orders.push(Order(buy.id, true, buy.price, buy.quantity - matched_qty));
                if (sell.quantity > matched_qty)
                    sell_orders.push(Order(sell.id, false, sell.price, sell.quantity - matched_qty));
            } else {
                break;
            }
        }
    }
};
