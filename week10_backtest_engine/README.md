# 📈 Week 10 – Quant Strategy Backtesting Engine

This week, you implemented a **multi-strategy backtesting engine** for quantitative trading, supporting realistic limit/market order matching, historical data replay, and detailed PnL analysis.

---

## 🚀 Features

- **Historical Market Data Replay**  
  - Loads tick or bar data from CSV (with timestamp & column names supported).
  - Replays each row as a market data tick into the simulation engine.

- **Multi-Strategy Backtesting**  
  - Supports multiple strategies in parallel:  
    - Market Making (MM)  
    - Momentum (MOM)  
    - Mean Reversion (MR)  
  - Each strategy maintains its own state, position, and PnL.

- **Realistic Limit Order Book Matching**  
  - Centralized matching engine (`OrderBook`) supports both market and limit orders.
  - Partial fills and price-time priority for real-world simulation.

- **PnL & Trade Logging**  
  - For each strategy, reports realized PnL, unrealized PnL, total PnL, position, and trade count at the end of the run.

---

## 📁 File Structure

| File                    | Description                                           |
|-------------------------|-------------------------------------------------------|
| `order.hpp`             | Order struct (symbol, side, price, qty, type)         |
| `orderbook.hpp`         | Limit order book & matching logic                     |
| `market_data.hpp`       | MarketData struct (symbol, last, high, low, avg, vol) |
| `market_data_loader.hpp`| Loader for CSV, supports header and timestamp         |
| `strategy.hpp`          | Strategy interface with PnL/position/reporting        |
| `market_making_strategy.hpp` | Market Making strategy                           |
| `momentum_strategy.hpp`       | Momentum strategy                               |
| `mean_reversion_strategy.hpp` | Mean Reversion strategy                         |
| `exchange.hpp`          | Simulated exchange: routes orders & data              |
| `main.cpp`              | Backtesting loop & summary                            |
| `generate_data.py`      | Python script to generate random AAPL data            |
| `AAPL_data.csv`         | Example historical data (generated or your own)       |

---

## 📝 Notes

- **Market data CSV** must follow the format:  
`timestamp,symbol,last_price,high,low,avg,volume`  
Header and timestamp are supported and will be skipped/ignored by the loader.

- **OrderBook** uses professional-grade logic:  
- Market orders sweep best price.
- Limit orders are matched at equal or better price, otherwise rest on the book.
- Partial fills are supported.

- **Extensibility**:  
- You can easily add new strategies, symbols, or connect to real datasets by modifying the loader.
- Output can be extended to CSV logs for further Python analysis.

- **PnL Calculation**:  
- Each strategy tracks realized and unrealized PnL.
- Final summary helps you compare strategy performance over the same historical scenario.

---

## 📊 Example Output

```
[MR] Order filled: AAPL BUY @99.95 x 2
[MOM] BUY signal @101.04
[MR] SELL signal @101.04 (mean=99.7)
[MR] BUY signal @99.01 (mean=99.68)
[MR] SELL signal @101.4 (mean=99.75)
OrderBook [AAPL]
 Asks (Sell):
   100.2 x 1
   100.32 x 3
   100.39 x 3
   100.46 x 3
   100.47 x 1
 Bids (Buy):
   99.62 x 1
   99.6 x 1
   99.53 x 1
   99.52 x 2
   99.5 x 1
====================

====== BACKTEST SUMMARY ======
MM  Realized PnL: 3299.45, Unrealized PnL: -0.4575  Final PnL: 3298.99, Trades: 53, Position: 4
MOM  Realized PnL: 3299.45, Unrealized PnL: -0.4575  Final PnL: 3298.99, Trades: 53, Position: 4
MR  Realized PnL: 3299.45, Unrealized PnL: -0.4575  Final PnL: 3298.99, Trades: 53, Position: 4
```