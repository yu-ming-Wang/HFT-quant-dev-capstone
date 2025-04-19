#pragma once
#include "order_book.hpp"
#include <unordered_map>
#include <string>
#include <iostream>
#include <queue>
#include <vector>
#include <memory>

class Exchange {
public:
    void add_order(std::shared_ptr<Order> order) {
        books[order->symbol].add_order(order);
    }

    bool cancel_order(const std::string& symbol, const std::string& order_id) {
        return books[symbol].cancel_order(order_id);
    }

    bool modify_order(const std::string& symbol, const std::string& order_id, double new_price, int new_qty) {
        return books[symbol].modify_order(order_id, new_price, new_qty);
    }

    void print_order_book(const std::string& symbol) {
        std::cout << "\n📘 OrderBook for " << symbol << ":\n";
        books[symbol].print();
    }


private:
    std::unordered_map<std::string, OrderBook> books;
};