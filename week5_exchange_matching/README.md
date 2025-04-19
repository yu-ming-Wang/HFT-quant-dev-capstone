# 📈 Week 5 - Multi-Symbol Exchange Matching Engine

This week, we extend our order matching engine to support **multiple products (symbols)** using an `Exchange` class that internally manages multiple `OrderBook` instances.

---

## 🚀 Features Implemented

- ✅ `Order` struct supports multiple symbols
- ✅ `Exchange` routes each order to its corresponding symbol’s `OrderBook`
- ✅ Each `OrderBook`:
  - Matches orders based on price-time priority
  - Supports cancel and modify
  - Prints outstanding buy/sell orders
  - Logs all executed trades in a trade log

---

## 📁 File Structure

| File            | Description |
|------------------|-------------|
| `order.hpp`      | Order struct with symbol, buy/sell, timestamp, and status |
| `order_book.hpp` | Order matching engine for a single symbol, includes TradeLog |
| `exchange.hpp`   | Manager for multiple order books by symbol |
| `main.cpp`       | Sample usage: adds, modifies, cancels, and prints results |

---

## 🧪 Example Usage

```
Exchange exchange;

// Add AAPL orders
exchange.add_order(std::make_shared<Order>("A1", "AAPL", true, 150.0, 10, ts++));
exchange.add_order(std::make_shared<Order>("A2", "AAPL", false, 149.0, 10, ts++));

// Modify and cancel
exchange.cancel_order("AAPL", "A1");
exchange.modify_order("AAPL", "A2", 148.5, 5);

// Print current state
exchange.print_order_book("AAPL");

```


```
[MATCH] A1 x A2 @ 149 x 10
[CANCEL] Order A1 marked as canceled.
[MODIFY] Canceling and replacing Order A2

📘 OrderBook for AAPL:
Buy Orders:
  [BUY]  A3 @151 x 5
Sell Orders:
  [SELL] A4 @150 x 3
Trade Log:
  [TRADE] A1 x A2 @149 x 10
  [TRADE] A3 x A4 @150 x 3
```