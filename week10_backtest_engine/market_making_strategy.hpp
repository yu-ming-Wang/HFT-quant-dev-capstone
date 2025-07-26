#pragma once
#include "strategy.hpp"
#include "exchange.hpp"
#include <random>
#include <thread>
#include <chrono>
#include <iostream>

class MarketMakingStrategy : public Strategy {
public:
    MarketMakingStrategy(const std::string& symbol)
        : symbol_(symbol), gen_(std::random_device{}()),
          position_(0), cash_(0.0), trade_count_(0),
          realized_pnl_(0.0), unrealized_pnl_(0.0), avg_cost_(0.0)
    {}

    void on_market_data(const MarketData& md) override {
        last_md_ = md;
        // 每次行情更新時，動態計算未實現損益
        unrealized_pnl_ = (position_ == 0) ? 0.0 : (md.last_price - avg_cost_) * position_;
    }

    void on_order_filled(const Order& order) override {
        if (order.is_buy) {
            // 買進：加總平均成本
            double total_cost = avg_cost_ * position_ + order.price * order.quantity;
            position_ += order.quantity;
            avg_cost_ = (position_ == 0) ? 0.0 : total_cost / position_;
            cash_ -= order.price * order.quantity;
        } else {
            // 賣出：計算已實現損益
            realized_pnl_ += (order.price - avg_cost_) * order.quantity;
            position_ -= order.quantity;
            cash_ += order.price * order.quantity;
            if (position_ == 0) avg_cost_ = 0.0; // 清倉歸零
        }
        ++trade_count_;
        std::cout << "[MM] Order filled: " << order.symbol
                  << " " << (order.is_buy ? "BUY" : "SELL")
                  << " @" << order.price << " x " << order.quantity << std::endl;
    }

    // Interface for summary
    double get_realized_pnl() const override { return realized_pnl_; }
    double get_unrealized_pnl() const override { return unrealized_pnl_; }
    double get_pnl() const override { return realized_pnl_ + unrealized_pnl_; }
    int get_trade_count() const override { return trade_count_; }
    int get_position() const override { return position_; }
    std::string get_name() const override { return "MM"; }

    void run() override {
        while (last_md_.last_price == 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        for (int i = 0; i < 10; ++i) {
            if (!exchange_) return;
            double bid = last_md_.last_price - 0.2;
            double ask = last_md_.last_price + 0.2;
            std::uniform_int_distribution<int> dist_qty(1, 3);
            exchange_->submit_order(Order{-1, symbol_, true,  bid, dist_qty(gen_)});
            exchange_->submit_order(Order{-1, symbol_, false, ask, dist_qty(gen_)});
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
private:
    std::string symbol_;
    MarketData last_md_;
    std::mt19937 gen_;
    int position_;
    double cash_;
    int trade_count_;
    double realized_pnl_;
    double unrealized_pnl_;
    double avg_cost_;
};
