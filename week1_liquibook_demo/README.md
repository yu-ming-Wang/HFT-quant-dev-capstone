# 🧠 Week 1 - Liquibook Matching Engine Demo

This is the first milestone in my 12-week journey to becoming an HFT Quant Developer. The goal for Week 1 was to build a minimal matching engine prototype using [Liquibook](https://github.com/enewhuis/liquibook), a C++-based open-source limit order book engine.

---

## 🎯 Objectives

- ✅ Set up the development environment (Linux/WSL2 + g++ + GitHub)
- ✅ Clone and build the Liquibook engine
- ✅ Create a custom `MyOrder` class compatible with Liquibook's interface
- ✅ Implement and register `OrderListener` and `TradeListener` for callbacks
- ✅ Write a `main.cpp` demo that simulates buy/sell order matching and prints real-time execution details
- ✅ Understand the structure and event-driven nature of high-performance matching systems

---

## 🔧 Matching Logic Overview

- Implemented a custom `MyOrder` class with required methods:
  - `is_buy()`, `price()`, `order_qty()`, `is_limit()`, `fill()`, etc.
- Created a `MyListener` class to receive and log callbacks from the engine:
  - `on_accept()`, `on_fill()`, `on_trade()`, `on_cancel()`
- Registered the listeners in the matching engine:
  ```cpp
  book.set_order_listener(&listener);
  book.set_trade_listener(&listener);

