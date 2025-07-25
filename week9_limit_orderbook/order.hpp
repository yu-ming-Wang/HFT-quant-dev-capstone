#pragma once
#include <string>

enum class OrderType { LIMIT, MARKET };

struct Order {
    int order_id;
    std::string symbol;
    bool is_buy;
    double price;         // only used for limit order
    int quantity;
    OrderType type;
    // timestamp/更多欄位可加
};
