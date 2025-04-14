#pragma once
#include <string>
#include <cstdint>

struct Order {
    std::string id;
    bool is_buy;
    double price;
    int quantity;
    uint64_t timestamp;
    bool canceled = false;

    Order(std::string i, bool b, double p, int q, uint64_t ts = 0)
        : id(i), is_buy(b), price(p), quantity(q), timestamp(ts) {}
};