#pragma once
#include "order.hpp"
#include "market_data.hpp"
#include "strategy.hpp"
#include <vector>
#include <functional>
#include <mutex>
#include <random>
#include <thread>
#include <atomic>
#include <map>

class Strategy;

class Exchange {
public:
    Exchange() : running_(true), order_id_seq_(1) {}

    void register_strategy(Strategy* s) {
        strategies_.push_back(s);
        s->set_exchange(this);
    }

    void start_market_data() {
        md_thread_ = std::thread([this](){
            std::mt19937 gen(std::random_device{}());
            std::uniform_real_distribution<double> dist(98, 102);
            for (int t = 0; t < 40 && running_; ++t) {
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
                double p = dist(gen);
                market_data_.last_price = p;
                market_data_.high = std::max(market_data_.high, p);
                market_data_.low = (market_data_.low==0) ? p : std::min(market_data_.low, p);
                market_data_.recent_prices.push_back(p);
                if (market_data_.recent_prices.size() > 20) market_data_.recent_prices.pop_front();
                double sum = 0;
                for (double v : market_data_.recent_prices) sum += v;
                market_data_.avg = market_data_.recent_prices.empty() ? p : sum / market_data_.recent_prices.size();
                // 通知所有策略
                for (auto s : strategies_) s->on_market_data(market_data_);
            }
        });
    }

    void submit_order(Order o) {
        std::lock_guard<std::mutex> lock(order_mutex_);
        o.id = order_id_seq_++;
        // 直接成交邏輯（如市價）
        market_data_.volume += o.quantity;
        // 通知策略
        for (auto s : strategies_) s->on_order_filled(o);
    }

    void stop() {
        running_ = false;
        if (md_thread_.joinable()) md_thread_.join();
    }

    MarketData get_market_data() const { return market_data_; }

private:
    std::vector<Strategy*> strategies_;
    MarketData market_data_;
    std::thread md_thread_;
    std::atomic<bool> running_;
    std::mutex order_mutex_;
    int order_id_seq_;
};
