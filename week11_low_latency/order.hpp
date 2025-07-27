#pragma once
#include <string>

enum class OrderType { LIMIT, MARKET };

struct Order {
    int order_id;
    std::string symbol;
    bool is_buy;
    double price;
    int quantity;
    OrderType type;

    // Default Constructor (for RingBuffer internal array)
    Order()
        : order_id(0), symbol(""), is_buy(false), price(0.0), quantity(0), type(OrderType::LIMIT) {}

    // Parameterized Constructor
    Order(int id, const std::string& sym, bool buy, double p, int qty, OrderType t)
        : order_id(id), symbol(sym), is_buy(buy), price(p), quantity(qty), type(t) {}
};
