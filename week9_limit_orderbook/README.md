# 📊 Week 9 – Limit Order Book (Supports Limit & Market Orders)

This week’s goal is to implement a robust **Limit Order Book (LOB)** in C++ that fully supports both **limit orders** and **market orders**.  
The matching engine uses professional price-time priority rules, supports partial fills, cancellation, book snapshots, and multi-symbol testing.

---

## 🚀 Features

- **Limit Order Book with true price-time priority**
    - Each price level is a FIFO queue (per price, per side)
- **Market Order Matching**
    - Market orders sweep the book, matching at the best available prices (supporting partial fills)
- **Order Cancellation**
    - Cancel an existing order by order ID
- **Order Book Snapshot**
    - Query the current state of the order book (best bid/ask, book depth)
- **Multi-Symbol Support (via demo)**
    - Easily create multiple independent books for different symbols (AAPL, GOOG, TSLA, etc.)
- **Console Log for all actions**
    - Add, match, cancel, and unfilled orders all print clearly

---

## 🗂️ File Structure

| File              | Description                                            |
|-------------------|--------------------------------------------------------|
| `order.hpp`       | Order struct (with support for limit & market order)   |
| `orderbook.hpp`   | Limit Order Book data structure & matching logic       |
| `main.cpp`        | Demo: places 5 buy/sell limit orders + market orders   |
| `build.sh`        | Build and run script                                   |
| `README.md`       | This file                                              |

---

## 🧪 Example Usage

```
// Create OrderBooks for 3 symbols
std::vector<std::string> symbols = {"AAPL", "GOOG", "TSLA"};
std::vector<std::unique_ptr<OrderBook>> books;
for (const auto& sym : symbols)
    books.push_back(std::make_unique<OrderBook>(sym));

// Add 5 buy/sell limit orders per symbol
for (int i = 0; i < symbols.size(); ++i) {
    double base = 100 + i * 10;
    for (int k = 0; k < 5; ++k) {
        books[i]->add_order(true, base + k * 0.5, 3 + k, OrderType::LIMIT);
        books[i]->add_order(false, base + 3 + k * 0.5, 2 + k, OrderType::LIMIT);
    }
}

// Place market orders for each symbol
for (int i = 0; i < symbols.size(); ++i) {
    books[i]->add_order(true, 0, 7, OrderType::MARKET);  // market buy
    books[i]->add_order(false, 0, 8, OrderType::MARKET); // market sell
}

// Print order book snapshots
for (const auto& b : books)
    b->print_book();

```

## 📝 Notes

- The order book uses STL containers (`std::map`, `std::deque`) for efficient and fair price-time priority matching.
- **Market orders** sweep the book, filling at best available prices, and may result in partial fills if there is insufficient liquidity.
- **Limit orders** stay on the book until fully filled or canceled.
- Orders can be canceled by their `order_id`; the system removes the order from the corresponding price level.
- Multi-symbol simulation is supported by instantiating one `OrderBook` per symbol.
- **Extensibility:** The engine can be extended with trade logs, order status query, order modification, time-in-force (IOC/FOK), or integration with real or historical market data by replacing demo logic in `main.cpp`.
