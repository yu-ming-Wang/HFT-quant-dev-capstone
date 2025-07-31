#include "market_feed.hpp"
#include "order.hpp"
#include "lockfree_ringbuffer.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <iostream>
#include <cstdlib>

extern std::atomic<bool> running; 
extern LockFreeRingBuffer<Order, 65536> order_queue;

MarketFeed::MarketFeed() 
    : enable_random_tick(false), feed_running(false){}   

// === Load CSV bars ===
void MarketFeed::load_csv(const std::string& filename) {
    bars.clear();
    std::ifstream fin(filename);
    std::string line;
    getline(fin, line); // skip header

    while (getline(fin, line)) {
        std::istringstream ss(line);
        MarketTick tick;
        std::string token;

        getline(ss, token, ','); tick.timestamp = std::stoll(token);
        getline(ss, token, ','); tick.symbol = token;
        getline(ss, token, ','); tick.open = std::stod(token);
        getline(ss, token, ','); tick.high = std::stod(token);
        getline(ss, token, ','); tick.low = std::stod(token);
        getline(ss, token, ','); tick.close = std::stod(token);
        getline(ss, token, ','); tick.volume = std::stod(token);

        bars.push_back(tick);
    }
    std::cout << "[DEBUG] Loaded " << bars.size() << " bars from CSV.\n";
}

// === Start feed ===
void MarketFeed::start(TickHandler cb, int interval_ms) {
    std::cout << "[DEBUG] MarketFeed::start() called. feed_running=" 
              << feed_running << " bars.size=" << bars.size() 
              << " enable_random_tick=" << enable_random_tick << std::endl;

    handler = cb;
    feed_running = true;
    //detach make it background thread
    std::thread(&MarketFeed::generate_ticks, this, interval_ms).detach();
}

// === Stop feed ===
void MarketFeed::stop() {
    feed_running = false;
}

// === Generate ticks ===
void MarketFeed::generate_ticks(int interval_ms) {
    std::cout << "[DEBUG] generate_ticks() started. bars.size=" 
              << bars.size() << " enable_random_tick=" << enable_random_tick << std::endl;

    // ✅ 如果 bars 有資料 → 先跑 CSV backtest
    if (!bars.empty()) {
        for (const auto& tick : bars) {
            if (!running) break;

            handler(tick);

            // simulate order
            Order buy(rand() % 100000, tick.symbol, true, tick.open, 100, OrderType::LIMIT);
            Order sell(rand() % 100000 + 100000, tick.symbol, false, tick.close, 100, OrderType::LIMIT);
            double px = tick.low + ((double)rand() / RAND_MAX) * (tick.high - tick.low);
            Order rand_order(rand() % 100000 + 200000, tick.symbol, rand() % 2, px, 50, OrderType::LIMIT);

            order_queue.push(buy);
            order_queue.push(sell);
            order_queue.push(rand_order);

            std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
        }

        std::cout << "[DEBUG] Finished processing bars. Stopping feed.\n";
        feed_running = false;
        ::running = false;
        return;
    }

    // ✅ 如果 bars 為空且 enable_random_tick=true → 進入隨機 tick 模式
    if (enable_random_tick) {
        std::cout << "[DEBUG] Entering RANDOM TICK mode...\n";
        while (feed_running && running) {
            MarketTick tick;
            tick.symbol = "AAPL";
            tick.open = 100.0 + (rand() % 1000) / 10.0;
            tick.high = tick.open + (rand() % 50) / 10.0;
            tick.low = tick.open - (rand() % 50) / 10.0;
            tick.close = tick.low + ((double)rand() / RAND_MAX) * (tick.high - tick.low);
            tick.volume = 1000 + rand() % 5000;
            tick.timestamp = time(nullptr) * 1000; // in ms

            handler(tick);

            // simulate order
            Order buy(rand() % 100000, tick.symbol, true, tick.open, 100, OrderType::LIMIT);
            Order sell(rand() % 100000 + 100000, tick.symbol, false, tick.close, 100, OrderType::LIMIT);
            double px = tick.low + ((double)rand() / RAND_MAX) * (tick.high - tick.low);
            Order rand_order(rand() % 100000 + 200000, tick.symbol, rand() % 2, px, 50, OrderType::LIMIT);

            order_queue.push(buy);
            order_queue.push(sell);
            order_queue.push(rand_order);

            std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
        }

        std::cout << "[DEBUG] Random tick mode stopped.\n";
        feed_running = false;
        ::running = false;
    } else {
        // ✅ 不允許隨機 tick → 直接退出
        std::cerr << "[INFO] Bars empty and random tick disabled. Exiting feed.\n";
        feed_running = false;
        ::running = false;
    }
}
