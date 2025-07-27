# ⚡ Week 11 – Low Latency Matching Engine

This week, you implemented a **low-latency order matching engine** optimized for **nanosecond-level** performance, featuring a **lock-free order queue**, **memory-mapped fast logging**, and **latency profiling** with percentile analysis.

---

## 🚀 Features

- **Lock-Free Order Queue**  
  Implements a `LockFreeRingBuffer` for high-throughput order ingestion without mutex locks.

- **Optimized OrderBook**  
  Lightweight `OrderBookOptimized` with simplified matching logic for speed.

- **Memory-Mapped Fast Logger**  
  `FastLogger` logs trades directly into a memory-mapped file to minimize I/O overhead.

- **Nanosecond Latency Profiling**  
  `Profiler` measures per-order matching latency at nanosecond resolution, reporting **p50, p90, p99, p99.9**.

- **Latency CSV Export**  
  All latency samples saved to `week11_latency.csv` for offline analysis.

---

## 📁 File Structure

| File                  | Description                                                      |
|-----------------------|------------------------------------------------------------------|
| `order.hpp`           | Defines `Order` struct with default and parameterized constructors |
| `orderbook_optimized.hpp` | Minimal low-latency order book implementation                  |
| `ring_buffer.hpp`     | Lock-free ring buffer template                                   |
| `fast_logger.hpp`     | Memory-mapped trade logger                                       |
| `profiling.hpp`       | High-resolution latency profiler                                 |
| `main.cpp`            | Engine entry point: generates orders, matches, logs latency      |
| `build.sh`            | Build & run script (`g++` optimized with `-O3 -march=native`)    |
| `.gitignore`          | Ignore rules for binaries/logs/temp files                        |
| `README.markdown`     | Project documentation                                            |

---

## ⚡ FastLogger – Principle

`FastLogger` uses **Memory-Mapped File (mmap)** to log trades directly to a mapped memory region. This avoids costly `write()` syscalls, significantly reducing I/O latency.

### ✅ How it Works
```
[User Space] ── memcpy() ─▶ [Mapped Memory]
                               │
                               ▼
                          [Kernel Buffer] ──▶ [Disk]
```


### ✅ Advantages
- 🚀 **Low latency**: no syscall per log entry
- ⚡ **Zero-copy**: directly writes to memory mapped file
- 🗂 **Batch flush**: OS handles delayed disk flush efficiently

### ⚠️ Considerations
- Must pre-allocate file size with `ftruncate`
- Writing beyond mapped region will overwrite memory
- Ideal for **HFT logging** where microsecond-level performance matters

---


## 📝 Notes

### 📊 Latency Percentiles
- **p50** → Median latency (50% of requests faster)  
- **p90** → 90% processed faster than this value  
- **p99** → Tail latency (top 1% slowest)  
- **p99.9** → Worst-case latency, critical for HFT performance  

### ⚡ Performance Insights
- Most orders are processed in **<100ns**.  
- Rare outliers (**p99.9**) exceed **10µs**, often due to **OS scheduling** or **page faults**.  
- Monitoring **tail latency** is essential in HFT, where microseconds directly impact profitability.  

### 🚀 Extensibility
Week12 will extend this engine to include:
- **Bid/Ask separation**  
- **Price-time priority**  
- **Multi-symbol order routing**  
- **Gateway + Market Data Feed Integration**  
- **External API connectivity** (e.g., Binance/Polygon.io)

