#include "gateway.hpp"
#include "engine_manager.hpp"
#include "portfolio_manager.hpp"
#include "market_feed.hpp"
#include "latency_monitor.hpp"
#include "trade_logger.hpp"
#include "lockfree_ringbuffer.hpp"

#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include <csignal>

LockFreeRingBuffer<Order, 65536> order_queue;  
std::atomic<bool> running(true);

void signal_handler(int) { running = false; }

int main(int argc, char* argv[]) {
    // 捕捉 Ctrl-C
    std::signal(SIGINT, signal_handler);

    bool enable_feed = true;
    if (argc > 1 && std::string(argv[1]) == "--no-feed") {
        enable_feed = false;
        std::cout << "[INFO] MarketFeed disabled (--no-feed)" << std::endl;
    }

    LatencyMonitor latency_monitor;
    EngineManager engines;
    PortfolioManager portfolio;
    TradeLogger trade_logger("results/logs/trades.csv");

    engines.set_portfolio_manager(&portfolio);
    engines.add_symbol("AAPL");
    engines.add_symbol("MSFT");
    engines.add_symbol("GOOG");

    //Open Gateway
    int port = 8787;
    Gateway gateway("tcp://*:8787", &engines);
    gateway.start();

    std::vector<std::unique_ptr<MarketFeed>> feeds;

    auto start_feed = [&](const std::string& symbol, const std::string& file, int interval) {
        auto f = std::make_unique<MarketFeed>();
        f->load_csv(file);
        f->start([&](const MarketTick& tick) {
            std::cout << "[TICK] " << tick.symbol 
                      << " ts=" << tick.timestamp
                      << " O="  << tick.open 
                      << " C="  << tick.close << std::endl;
        }, interval);
        feeds.push_back(std::move(f));
    };

    // enable_feed = true start MarketFeed
    if (enable_feed) {
        start_feed("AAPL", "backtest_data/marketdata_AAPL_20250714_20250718.csv", 5);
        start_feed("MSFT", "backtest_data/marketdata_MSFT_20250714_20250718.csv", 15);
        start_feed("GOOG", "backtest_data/marketdata_GOOG_20250714_20250718.csv", 8);
    }

    //matching engnine
    std::thread matcher([&](){
        while (running) {
            while (auto order_opt = order_queue.pop()) {
                engines.submit_order(*order_opt);
            }

            auto t0 = std::chrono::high_resolution_clock::now();
            auto trades = engines.match_all(&latency_monitor);
            auto t1 = std::chrono::high_resolution_clock::now();
            long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
            latency_monitor.record(ns);

            for (const auto& t : trades) {
                trade_logger.log_trade(t);
                std::cout << "[TRADE] buy=" << t.buy_order_id
                          << " sell=" << t.sell_order_id
                          << " sym=" << t.symbol
                          << " px=" << t.price
                          << " qty=" << t.quantity << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    });

    std::cout << "HFT Engine running on tcp://localhost:" << port << std::endl;
    std::cout << "Press Ctrl-C to stop.\n";

    while (running) std::this_thread::sleep_for(std::chrono::milliseconds(100));

    for (auto& f : feeds) f->stop();
    gateway.stop();
    matcher.join();

    latency_monitor.export_csv("results/latency/latency.csv");
    latency_monitor.export_symbol_csv("results/latency/latency_per_symbol.csv");
    portfolio.export_csv("results/pnl/portfolio_pnl.csv");

    std::cout << "[DONE] Week12 Portfolio Infra Engine Exit.\n";
    return 0;
}
