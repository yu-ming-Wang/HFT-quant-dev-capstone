#include "exchange.hpp"
#include "strategy.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <memory>
//Self Build Strategy
#include "market_making_strategy.hpp" 
#include "momentum_strategy.hpp"
#include "mean_reversion_strategy.hpp"

int main() {
    Exchange ex;
    std::vector<std::unique_ptr<Strategy>> strategies;
    strategies.push_back(std::make_unique<MarketMakingStrategy>("AAPL"));
    strategies.push_back(std::make_unique<MomentumStrategy>("AAPL"));
    strategies.push_back(std::make_unique<MeanReversionStrategy>("AAPL"));

    // 註冊策略
    for (auto& s : strategies) ex.register_strategy(s.get());

    ex.start_market_data();

    // Open strategy thread
    std::vector<std::thread> threads;
    for (auto& s : strategies) {
        threads.emplace_back([&]{ s->run(); });
    }

    for (auto& t : threads) t.join();
    ex.stop();


    // Print out Summary
    std::cout << "\n================== STRATEGY SUMMARY ==================\n";
    for (const auto& s : strategies) {
        std::cout << "------ [" << s->get_name() << "] ------" << std::endl;
        std::cout << "  Realized PnL:   " << s->get_realized_pnl() << std::endl;
        std::cout << "  Unrealized PnL: " << s->get_unrealized_pnl() << std::endl;
        std::cout << "  Total   PnL:    " << s->get_pnl() << std::endl;
        std::cout << "  Trade count:    " << s->get_trade_count() << std::endl;
        std::cout << "  Final position: " << s->get_position() << std::endl;
        std::cout << std::endl;
    }
    std::cout << "=====================================================\n";
    std::cout << "Week 8 HFT strategy demo finished.\n";
    return 0;
}
