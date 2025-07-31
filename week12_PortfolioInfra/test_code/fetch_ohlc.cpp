#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

//g++ fetch_ohlc.cpp -o fetch_ohlc -lcpr -std=c++17
//compile instruction

int main() {
    std::string api_key = "gb29Q9SEfSM7dpph3BHBlhY3wA4uyLyi";
    std::string symbol = "GOOG";
    std::string start_time = "2025-07-14";
    std::string end_time = "2025-07-18";  
    std::string url = "https://api.polygon.io/v2/aggs/ticker/" + symbol +
                      "/range/1/minute/" + start_time + "/" + end_time +
                       "?adjusted=true&sort=asc&apiKey=" + api_key;

    auto r = cpr::Get(cpr::Url{url});
    std::cout << "status=" << r.status_code << "\n";

    if (r.status_code == 200) {
        auto j = nlohmann::json::parse(r.text);
        if (j.contains("results")) {
            // === open CSV fileand write header ===
            std::ofstream csv("marketdata_" + symbol + "_20250714_20250718.csv");
            csv << "timestamp,symbol,open,high,low,close,volume\n";

            // === write out minute bar ===
            for (auto& bar : j["results"]) {
                double open = bar["o"];
                double close = bar["c"];
                double high = bar["h"];
                double low = bar["l"];
                double volume = bar["v"];
                int64_t ts = bar["t"];

                csv << ts << "," << symbol << "," 
                    << open << "," << high << "," << low << "," 
                    << close << "," << volume << "\n";
            }
            csv.close();
            std::cout << "[OK] Data saved to marketdata_"+ symbol +"_20250714_20250718.csv\n";
        } else {
            std::cout << "No results, raw: " << r.text << std::endl;
        }
    } else {
        std::cout << "HTTP error: " << r.status_code << ", msg: " << r.error.message << std::endl;
    }
}
