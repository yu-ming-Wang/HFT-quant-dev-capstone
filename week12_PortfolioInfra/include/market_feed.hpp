#pragma once
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>

// We accept new market source and change to markettick pass to engine 
struct MarketTick {
    std::string symbol;
    double open, high, low, close;
    double volume;
    int64_t timestamp;
};

class MarketFeed {
public:
    using TickHandler = std::function<void(const MarketTick&)>;
    MarketFeed();
    void load_csv(const std::string& filename); 
    void start(TickHandler cb, int interval_ms=100);
    void stop();

    bool enable_random_tick;  

private:
    void generate_ticks(int interval_ms);
    std::atomic<bool> feed_running;
    TickHandler handler;
    std::vector<MarketTick> bars; // 如果有資料就 replay
};
