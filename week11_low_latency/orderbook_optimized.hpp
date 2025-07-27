#pragma once
#include "order.hpp"
#include <vector>
#include <algorithm>

struct PriceLevel {
    double price;
    std::vector<Order> orders;
};

class OrderBookOptimized {
public:
    void add_order(const Order& o) {
        auto it = std::lower_bound(levels.begin(), levels.end(), o.price,
            [](const PriceLevel& pl, double price){ return pl.price < price; });

        if (it == levels.end() || it->price != o.price) {
            it = levels.insert(it, {o.price, {}});
        }
        it->orders.push_back(o);
    }

    std::vector<Order> match(const Order& incoming) {
        std::vector<Order> fills;
        if (levels.empty()) return fills;

        auto& lvl = levels.front();
        while (!lvl.orders.empty() && incoming.quantity > 0) {
            Order matched = lvl.orders.back();
            lvl.orders.pop_back();
            fills.push_back(matched);
        }
        return fills;
    }

private:
    std::vector<PriceLevel> levels;
};
