# 🏎️ Week 6 - Multi-Threaded Multi-Symbol Matching Engine

This week, we upgrade our exchange engine to a **multi-threaded architecture** where each product (symbol) is handled by its own matching thread and thread-safe queue. This design lays the foundation for real-world high-performance order matching in a multi-symbol trading environment.

---

## 🚀 Features Implemented

- ✅ Each symbol (e.g. AAPL, GOOG, MSFT) is managed by an independent matching thread
- ✅ All order queues are thread-safe to guarantee no lost or duplicated orders under concurrency
- ✅ Orders are randomly generated and routed to the correct symbol’s queue by the main thread
- ✅ Each matching thread continuously pops orders from its queue and simulates the matching process
- ✅ Atomic flags are used for thread-safe and graceful thread termination

---

## 📁 File Structure

| File                   | Description |
|------------------------|-------------|
| `exchange_runner.cpp`  | Main entry point. Sets up queues, spawns matching threads, and generates orders |
| `thread_safe_queue.hpp`| Minimal thread-safe queue implementation (mutex + condition variable) |
| `build.sh`             | Build and run script for the project |
| `README.md`            | Project documentation (this file) |

---

## 🧪 Example Usage

```
// exchange_runner.cpp main workflow

// 1. Start a matching thread and queue for each symbol
std::vector<std::string> symbols = {"AAPL", "GOOG", "MSFT"};
for (const auto& symbol : symbols) {
    // Each symbol gets its own queue and thread
    // orderbook_worker runs in a separate thread
}

// 2. Randomly generate and dispatch orders to each symbol's queue
Order order{order_id++, symbol, random_price, random_qty};
queues[symbol].push(order);

// 3. Each matching thread pops from its queue and simulates matching
// (In this version, matching is simulated by printing out order info)
```


```
Sample Output:

[MATCH] Symbol: MSFT, OrderID: 3, Price: 184.955, Qty: 2
[MATCH] Symbol: GOOG, OrderID: 6, Price: 156.53, Qty: 9
[MATCH] Symbol: AAPL, OrderID: 7, Price: 117.758, Qty: 9
...
All matching threads stopped.
'''
```


## 📝 Notes

- Each symbol is isolated in its own thread and queue, ensuring **no data races, no lost or duplicated orders**, and correct concurrent operation.
- The current implementation simulates matching; real matching logic (price-time priority, trade log, etc.) can be added to each worker thread.
- Atomic flags allow the main thread to **gracefully shut down** all matching threads.
- **Thread-safe queue vs. shared memory:**  
  - A thread-safe queue is designed for safe data sharing between threads within the same process, using synchronization primitives (mutexes, locks) to prevent race conditions.
  - Shared memory is intended for sharing data across different processes (IPC), and requires manual synchronization to ensure data consistency.

