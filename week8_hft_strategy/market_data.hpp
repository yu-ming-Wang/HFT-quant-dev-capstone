#pragma once
#include <string>
#include <deque>

struct MarketData {
    std::string symbol;
    double last_price = 0;
    double high = 0;
    double low = 0;
    double avg = 0;
    int volume = 0;
    std::deque<double> recent_prices; // 可選：滑動均價用
};
