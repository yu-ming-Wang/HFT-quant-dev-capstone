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
};
