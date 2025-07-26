#pragma once
#include "order.hpp"
#include "market_data.hpp"

class Exchange; // Forward declaration

class Strategy {
public:
    virtual void on_market_data(const MarketData& md) = 0;
    virtual void on_order_filled(const Order& order) = 0;
    virtual void run() = 0;

    // get pnl
    virtual double get_pnl() const = 0;                
    virtual double get_realized_pnl() const = 0;       
    virtual double get_unrealized_pnl() const = 0;     

    virtual int get_trade_count() const = 0;
    virtual int get_position() const = 0;
    virtual std::string get_name() const = 0;

    virtual ~Strategy() {}

protected:
    Exchange* exchange_ = nullptr;

public:
    void set_exchange(Exchange* ex) { exchange_ = ex; }
};
