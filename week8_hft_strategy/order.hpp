#pragma once
#include <string>

struct Order {
    int id;
    std::string symbol;
    bool is_buy;
    double price;
    int quantity;
};
