#pragma once
#include <fstream>
#include <string>
#include <mutex>
#include "trade.hpp"

class TradeLogger {
public:
    TradeLogger(const std::string& filename) : file(filename, std::ios::app) {}
    void log_trade(const Trade& t) {
        std::lock_guard<std::mutex> lock(mtx);
        file << t.buy_order_id << "," << t.sell_order_id << "," << t.symbol
             << "," << t.price << "," << t.quantity << "\n";
    }
    ~TradeLogger() { file.close(); }
private:
    std::ofstream file;
    std::mutex mtx;
};
