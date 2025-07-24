# ⚡️ Week 7 - Stress Testing, Performance Profiling, and Correctness Validation

In Week 7, we focus on **stress testing** and **validating correctness** of the multi-threaded, multi-symbol matching engine under high concurrency. We simulate heavy order flow, measure system throughput and latency, and ensure that all orders are processed exactly once without any loss or duplication.

---

## 🚀 Features and Experiments

- ✅ Multi-threaded stress test with thousands of orders per symbol using multiple producer threads
- ✅ Each symbol has its own dedicated matching thread and thread-safe queue
- ✅ Automatic correctness validation: every order ID pushed to the system is tracked and checked against matched orders
- ✅ Throughput (orders per second) and average latency measurement
- ✅ Console output is synchronized to prevent print overlap (using a global mutex)
- ✅ Ready for lock-free queue or further optimization if required

---

## 📁 File Structure

| File                    | Description                                                      |
|-------------------------|------------------------------------------------------------------|
| `stress_test.cpp`       | Main stress test: order generation, matching, correctness check  |
| `thread_safe_queue.hpp` | Minimal thread-safe queue implementation (mutex + cond_var)      |
| `build.sh`              | Build and run script                                             |
| `README.md`             | This documentation                                               |

---



## 🧪 How to Run

```
chmod +x build.sh
./build.sh

```

```
Sample Output:

[MATCH] Symbol: GOOG, OrderID: 9212, Latency: 1.98 ms
[MATCH] Symbol: AAPL, OrderID: 11234, Latency: 2.08 ms
...
[RESULT] Total pushed: 20000, matched: 20000
✅ All orders matched!
[STATS] Throughput: 30769.2 orders/sec, Avg Latency: 1.98 ms

```

## 📝 Notes

- Each producer thread generates and dispatches orders randomly to different symbols.
- Each symbol's worker thread processes orders from its queue concurrently.
- **Correctness validation:** The code checks that every pushed order ID is matched exactly once (no order is lost or duplicated).
- All console output is synchronized with a global mutex to avoid output interleaving.
- **Thread-safe queue vs. shared memory:**
  - Thread-safe queue is for data sharing among threads in the same process, with built-in synchronization (mutex/lock).
  - Shared memory is used for inter-process communication (IPC), and requires manual synchronization.

---

## 🏁 Checklist

- [x] Implemented high-concurrency stress test for multi-threaded matching engine
- [x] Automatic throughput and latency measurement
- [x] Comprehensive order ID correctness validation
- [x] Synchronized console logging to avoid race condition in output
- [ ] (Do in the future) Try lock-free queue for advanced optimization
- [ ] (Do in the future) Analyze lock contention and system bottlenecks

