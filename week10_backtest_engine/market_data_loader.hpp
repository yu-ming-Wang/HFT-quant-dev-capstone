#pragma once
#include "market_data.hpp"
#include <vector>
#include <fstream>
#include <sstream>

inline std::vector<MarketData> load_market_data(const std::string& filename) {
    std::vector<MarketData> result;
    std::ifstream fin(filename);
    std::string line;
    bool is_first_line = true;

    while (std::getline(fin, line)) {
        // 跳過 header（第一行）: 有 colname 就跳，沒有也不會影響
        if (is_first_line) {
            // 判斷第一行是不是 header，簡單判斷字串有 symbol 就跳過
            if (line.find("symbol") != std::string::npos || line.find("last_price") != std::string::npos) {
                is_first_line = false;
                continue;
            }
            is_first_line = false;
        }

        std::istringstream iss(line);
        MarketData md;
        std::string token;

        // timestamp
        std::getline(iss, token, ','); // skip this col
        // symbol
        std::getline(iss, md.symbol, ',');
        // last_price
        std::getline(iss, token, ','); md.last_price = std::stod(token);
        // high
        std::getline(iss, token, ','); md.high = std::stod(token);
        // low
        std::getline(iss, token, ','); md.low = std::stod(token);
        // avg
        std::getline(iss, token, ','); md.avg = std::stod(token);
        // volume
        std::getline(iss, token, ','); md.volume = std::stoi(token);

        result.push_back(md);
    }
    return result;
}
