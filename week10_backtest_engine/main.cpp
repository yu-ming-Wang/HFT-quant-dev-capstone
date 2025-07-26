#include "exchange.hpp"
#include "market_making_strategy.hpp"
#include "momentum_strategy.hpp"
#include "mean_reversion_strategy.hpp"
#include "market_data_loader.hpp"
#include <vector>
#include <memory>
#include <iostream>

int main() {
    std::vector<std::string> symbols = {"AAPL"};
    Exchange ex(symbols);

    std::vector<std::unique_ptr<Strategy>> strategies;
    strategies.push_back(std::make_unique<MarketMakingStrategy>("AAPL"));
    strategies.push_back(std::make_unique<MomentumStrategy>("AAPL"));
    strategies.push_back(std::make_unique<MeanReversionStrategy>("AAPL"));
    for (auto& s : strategies) ex.register_strategy(s.get());

    auto market_data = load_market_data("AAPL_data.csv");
    for (const auto& md : market_data) {
        ex.broadcast_market_data(md); // 播行情給所有策略
    }

    ex.print_all_books();

    std::cout << "\n====== BACKTEST SUMMARY ======\n";
    for (const auto& s : strategies) {
        std::cout << s->get_name()
                  << "  Realized PnL: " << s->get_realized_pnl()
                  << ", Unrealized PnL: " << s->get_unrealized_pnl()
                  << "  Final PnL: " << s->get_pnl()
                  << ", Trades: " << s->get_trade_count()
                  << ", Position: " << s->get_position() << std::endl;
    }
    return 0;
}
