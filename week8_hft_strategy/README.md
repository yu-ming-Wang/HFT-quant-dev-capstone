# 📈 Week 8 – Multi-Strategy C++ Trading Engine (PnL & Market Simulation)

This week’s project focuses on building a **multi-strategy, multi-threaded C++ trading engine** that features both realized and unrealized PnL tracking.  
Market data is generated in real-time by the `Exchange`’s internal simulator. All strategies receive these simulated ticks and independently calculate performance metrics.

---

## 🚀 Features

- ✅ **Modular Strategy Design:** Market Making, Momentum, and Mean Reversion strategies, each as a subclass of the common `Strategy` base class
- ✅ **Multi-threaded Execution:** Each strategy runs in its own thread, simulating real HFT environments
- ✅ **PnL Tracking:** Each strategy reports **realized**, **unrealized**, and **total** PnL automatically
- ✅ **Automatic Performance Summary:** Prints a detailed summary for each strategy after execution
- ✅ **Market Data Simulator:** The `Exchange` class generates random market prices at each tick and broadcasts to all strategies
- ✅ **Clean OOP Architecture:** Easy to extend for additional strategies, symbols, or new data sources

---

## 📁 File Structure

| File                         | Description                                        |
|------------------------------|----------------------------------------------------|
| `order.hpp`                  | Order struct                                       |
| `market_data.hpp`            | MarketData struct                                  |
| `strategy.hpp`               | Strategy interface with PnL summary methods        |
| `market_making_strategy.hpp` | MarketMaking strategy implementation               |
| `momentum_strategy.hpp`      | Momentum strategy implementation                   |
| `mean_reversion_strategy.hpp`| Mean Reversion strategy implementation             |
| `exchange.hpp`               | Matching engine and market data broadcaster        |
| `strategy_demo.cpp`          | Main program: registers strategies and runs engine |
| `build.sh`                   | Build and run script                               |
| `README.md`                  | This file                                          |

---

## 🧪 Example Usage

```
// Register strategies
strategies.push_back(std::make_unique<MarketMakingStrategy>("AAPL"));
strategies.push_back(std::make_unique<MomentumStrategy>("AAPL"));
strategies.push_back(std::make_unique<MeanReversionStrategy>("AAPL"));
// Start each strategy in its own thread
for (auto& s : strategies) ex.register_strategy(s.get());
ex.start_market_data();
for (auto& s : strategies) threads.emplace_back([&]{ s->run(); });
for (auto& t : threads) t.join();
ex.stop();
```

## 🧪 How to Run

```
chmod +x build.sh
./build.sh

```

```
Sample Output:

[MM] Order filled: AAPL SELL @100.899 x 1
[MOM] Order filled: AAPL SELL @100.899 x 1
[MR] Order filled: AAPL SELL @100.899 x 1
[MR] SELL signal @100.899 (mean=99.9068)
[MM] Order filled: AAPL BUY @98.7957 x 1
[MOM] Order filled: AAPL BUY @98.7957 x 1
[MR] Order filled: AAPL BUY @98.7957 x 1
[MR] BUY signal @98.7957 (mean=100.165)
[MM] Order filled: AAPL SELL @101.433 x 1
[MOM] Order filled: AAPL SELL @101.433 x 1
[MR] Order filled: AAPL SELL @101.433 x 1
[MR] SELL signal @101.433 (mean=100.152)

================== STRATEGY SUMMARY ==================
------ [MM] ------
  Realized PnL:   15.0313
  Unrealized PnL: 7.80268
  Total   PnL:    22.834
  Trade count:    38
  Final position: 3

------ [MOM] ------
  Realized PnL:   15.0313
  Unrealized PnL: 7.80268
  Total   PnL:    22.834
  Trade count:    38
  Final position: 3

------ [MR] ------
  Realized PnL:   15.0313
  Unrealized PnL: 7.80268
  Total   PnL:    22.834
  Trade count:    38
  Final position: 3

=====================================================
Week 8 HFT strategy demo finished.
```

## 📝 Notes

- Each strategy instance manages its own PnL (realized, unrealized, total) and runs in a separate thread.
- Market data is simulated by the `Exchange` class using a random price generator at each tick.
- Summary reports are printed automatically after all strategies complete.
- **PnL separation:**  
  - *Realized PnL* = net result from closed trades  
  - *Unrealized PnL* = current mark-to-market of open positions
- **To switch to real-world market data:**  
  - Simply replace the market data simulation logic in the `Exchange` class with code that reads from HDF5/CSV or live feeds.  
  - All strategy logic and summary reporting will continue to work unchanged.

---
