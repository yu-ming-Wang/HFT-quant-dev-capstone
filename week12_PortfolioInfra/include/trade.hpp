#pragma once
#include <string>

struct Trade {
    int buy_order_id;
    int sell_order_id;
    std::string symbol;
    double price;
    int quantity;
};
