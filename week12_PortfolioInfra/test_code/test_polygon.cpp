#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <iostream>

int main() {
    std::string api_key = "gb29Q9SEfSM7dpph3BHBlhY3wA4uyLyi";
    std::string symbol = "AAPL"; // 你要查的股票
    std::string url = "https://api.polygon.io/v2/last/trade/" + symbol + "?apiKey=" + api_key;

    auto r = cpr::Get(cpr::Url{url});
    if (r.status_code == 200) {
        auto j = nlohmann::json::parse(r.text);
        if (j.contains("results")) {
            double price = j["results"]["p"];
            int64_t ts = j["results"]["t"];
            std::cout << symbol << " last price: " << price << ", timestamp: " << ts << std::endl;
        } else {
            std::cout << "JSON parse error, raw: " << r.text << std::endl;
        }
    } else {
        std::cout << "HTTP error: " << r.status_code << std::endl;
    }
}
