# 📈 Week 12: High-Frequency Trading Portfolio Infrastructure

## ✅ Overview

This project implements a **high-performance trading engine infrastructure** featuring:
- **Multi-client Gateway** using **ZeroMQ ROUTER/DEALER** pattern  
- **Lock-free Order Queue** for efficient order passing  
- **Multi-threaded Worker Pool** for order processing  
- **Optimized Matching Engine** with per-symbol OrderBooks  
- **Latency Monitoring** and **Portfolio PnL Tracking**  
- **Market Feed** for backtesting or live simulation (can be disabled with `--no-feed`)

This is the **final milestone** for the Week 12 project, successfully completing the HFT Portfolio Infrastructure module.

---

## 🗂️ File Structure

| File                        | Description                                                                                             |
|-----------------------------|---------------------------------------------------------------------------------------------------------|
| `order.hpp`                 | Defines the `Order` struct, including order ID, symbol, side, price, quantity, and type (LIMIT/MARKET). |
| `trade.hpp`                 | Defines the `Trade` struct used to record matched trades.                                               |
| `lockfree_ringbuffer.hpp`   | Implements a lock-free ring buffer for passing orders between threads efficiently.                      |
| `orderbook_optimized.hpp`   | Optimized per-symbol limit order book with price-time priority matching.                                |
| `gateway.cpp`               | Implements the ZeroMQ multi-client Gateway (ROUTER → DEALER → Worker Pool).                             |
| `engine_manager.cpp`        | Manages multiple order books, processes incoming orders, and executes matching logic.                   |
| `portfolio_manager.cpp`     | Tracks portfolio PnL for all symbols and outputs PnL CSV results.                                       |
| `latency_monitor.cpp`       | Measures and logs latency per trade and per symbol.                                                     |
| `market_feed.cpp`           | Simulates a market feed by replaying historical CSV data as market ticks.                               |
| `main.cpp`                  | Entry point: initializes components, starts Gateway, Workers, and optional Market Feed.                 |
| `multi_client_demo.py`      | Python script to simulate multiple clients sending random buy/sell orders to the Gateway.               |
| `README.md`                 | Documentation of the Week 12 Portfolio Infrastructure project.                                          |

---

## 🚀 Features Implemented

### ✅ Gateway (Multi-Client Order Ingress)
- Supports **multiple clients** concurrently via **ZeroMQ ROUTER**.
- Each client has a unique identity → orders are routed correctly.
- ACK responses are correctly sent back to each client.

### ✅ Worker Pool (Parallel Processing)
- Backend uses a **DEALER socket** bound to `inproc://yuming_slaves`.
- Multiple worker threads **connect** to this endpoint.
- Workers receive `[identity][payload]`, parse orders, and send ACKs back with identity.

### ✅ Matching Engine
- Per-symbol OrderBook (`OrderBookOptimized`) supporting limit order matching.
- Matching logic follows **price-time priority**.
- Generates trades (`[TRADE] buy=... sell=... sym=... px=... qty=...`).

### ✅ Market Feed (Replay & Simulation)
- Reads historical CSV (minute bars) and generates simulated orders.
- Can be disabled with the `--no-feed` flag for clean client testing.

### ✅ Portfolio Management
- Tracks **PnL per symbol**.
- Outputs CSV with final portfolio state.

### ✅ Latency Monitoring
- Captures and logs **per-match latency** and **per-symbol latency**.

---

## 🖥 Project Structure



## 📊 Example Output


```
Expected Client Output:

[C1] sending: ORDER|6571|AAPL|1|154.65|100|LIMIT
[C1] got reply: ACK
[C3] sending: ORDER|2347|AAPL|0|152.4|100|LIMIT
[C3] got reply: ACK
```


```
Expected Engine Output:

[TRADE] buy=1819 sell=6643 sym=AAPL px=151.22 qty=50
[TRADE] buy=4336 sell=7112 sym=AAPL px=149.80 qty=50
```

```
Sample Logs:

timestamp,latency_ns
2025-07-29T17:07:00,125000

```

## 🧩 How the ZeroMQ Gateway Works

### Message Flow
1. **Client DEALER** → sends `[ORDER|...]` with its identity (`C1`).
2. **ROUTER Frontend** → preserves identity and forwards to Backend DEALER.
3. **Worker DEALER** → receives `[identity][payload]`, processes payload, pushes to order queue, and returns `[identity][ACK]`.
4. **ROUTER** → routes ACK back to the correct client.

---

## ✅ Key Technical Decisions
- **ROUTER/DEALER** → Enables multi-client, multi-worker non-blocking communication.
- **LockFree RingBuffer** → Low latency, thread-safe order passing.
- **Per-symbol OrderBook** → Supports concurrent matching across multiple instruments.
- **Modular Design** → Ready for expansion (Market Data Broadcaster, Web UI, etc.).

---
## 🗒️ Notes

- **Gateway Ports**  
  - The Gateway binds to `tcp://*:8787` (or your configured port).  
  - Use `lsof -i :8787` to check if the port is already in use.  
  - If `Address already in use` occurs, kill any leftover process with `kill -9 <PID>`.

- **Worker Identity Handling**  
  - Workers must **receive two frames**: `[identity][payload]`.  
  - They must **send back the identity frame** with ACK to allow proper client routing.  
  - This follows the ZeroMQ ROUTER/DEALER message pattern.

- **Market Feed Control**  
  - Use the flag `--no-feed` to disable simulated market data during testing.  
  - This is useful for clean Gateway → Worker → Engine multi-client testing.

- **Testing Order Matching**  
  - Python demo generates random buy/sell orders to trigger trades.  
  - Engine logs `[TRADE]` only when matching occurs (buy price ≥ sell price).  

- **Performance Considerations**  
  - Lock-free ring buffer minimizes contention in order passing.  
  - The system supports scaling with multiple worker threads to handle high load.

---
## 🔥 Next Steps (Mini Exchange Roadmap)
- Add **Market Data Broadcaster** (ZeroMQ PUB/SUB).
- Implement **WebSocket Dashboard** to visualize live OrderBook.
- Introduce **Risk Engine** (order validation & throttling).
- Add **Persistence Layer** (TimescaleDB or PostgreSQL for trade history).






