#pragma once
#include <string>
#include <cstdint>

struct Order {
    std::string id;
    std::string symbol; // 🆕 新增 symbol 欄位
    bool is_buy;
    double price;
    int quantity;
    uint64_t timestamp;
    bool canceled = false;

    Order(std::string i, std::string sym, bool b, double p, int q, uint64_t ts = 0)
        : id(i), symbol(sym), is_buy(b), price(p), quantity(q), timestamp(ts) {}
};