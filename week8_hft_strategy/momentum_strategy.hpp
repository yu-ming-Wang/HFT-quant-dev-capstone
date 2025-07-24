#pragma once
#include "strategy.hpp"
#include <thread>
#include <chrono>
#include <iostream>

class MomentumStrategy : public Strategy {
public:
    MomentumStrategy(const std::string& symbol)
        : symbol_(symbol), has_position_(false), position_(0), cash_(0.0), trade_count_(0),
          realized_pnl_(0.0), unrealized_pnl_(0.0), avg_cost_(0.0) {}

    void on_market_data(const MarketData& md) override {
        last_md_ = md;
        // 更新未實現損益
        unrealized_pnl_ = (position_ == 0) ? 0.0 : (md.last_price - avg_cost_) * position_;

        // 策略信號
        if (!has_position_ && md.last_price > md.high - 0.1) {
            if (exchange_) {
                exchange_->submit_order(Order{-1, symbol_, true, md.last_price, 2});
                has_position_ = true;
                std::cout << "[MOM] BUY signal @" << md.last_price << std::endl;
            }
        }
        if (has_position_ && md.last_price < md.low + 0.1) {
            if (exchange_) {
                exchange_->submit_order(Order{-1, symbol_, false, md.last_price, 2});
                has_position_ = false;
                std::cout << "[MOM] SELL signal @" << md.last_price << std::endl;
            }
        }
    }

    void on_order_filled(const Order& order) override {
        if (order.is_buy) {
            double total_cost = avg_cost_ * position_ + order.price * order.quantity;
            position_ += order.quantity;
            avg_cost_ = (position_ == 0) ? 0.0 : total_cost / position_;
            cash_ -= order.price * order.quantity;
        } else {
            realized_pnl_ += (order.price - avg_cost_) * order.quantity;
            position_ -= order.quantity;
            cash_ += order.price * order.quantity;
            if (position_ == 0) avg_cost_ = 0.0;
        }
        ++trade_count_;
        std::cout << "[MOM] Order filled: " << order.symbol
                  << " " << (order.is_buy ? "BUY" : "SELL")
                  << " @" << order.price << " x " << order.quantity << std::endl;
    }

    void run() override {
        std::this_thread::sleep_for(std::chrono::seconds(7));
    }

    // 三種 PnL summary
    double get_pnl() const override { return realized_pnl_ + unrealized_pnl_; }
    double get_realized_pnl() const override { return realized_pnl_; }
    double get_unrealized_pnl() const override { return unrealized_pnl_; }
    int get_trade_count() const override { return trade_count_; }
    int get_position() const override { return position_; }
    std::string get_name() const override { return "MOM"; }

private:
    std::string symbol_;
    MarketData last_md_;
    bool has_position_;
    int position_;
    double cash_;
    int trade_count_;
    double realized_pnl_;
    double unrealized_pnl_;
    double avg_cost_;
};
