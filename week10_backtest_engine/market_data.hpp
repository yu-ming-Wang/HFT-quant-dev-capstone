#pragma once
#include <string>

struct MarketData {
    std::string symbol;
    double last_price;
    double high;
    double low;
    double avg;
    int volume;
    // timestamp 可擴充
};
