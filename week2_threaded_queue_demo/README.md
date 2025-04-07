# Week 2: Multi-threaded Order Processing with Thread-Safe Queue

## ✅ Goal
In this week, I implemented a simulation of a high-frequency trading (HFT) environment where multiple producers and consumers interact through a shared thread-safe order queue. This lays the groundwork for building a scalable market data processing and matching engine pipeline.

## 📦 Files
- `main.cpp`: Contains the main simulation program that spawns multiple producer and consumer threads.
- `thread_safe_queue.hpp`: A custom thread-safe queue class implemented using `std::mutex` and `std::condition_variable`.

## 🧠 Concepts Practiced
- **Multithreading with std::thread**
- **Producer-consumer concurrency model**
- **Race condition prevention using mutex locking**
- **Shared memory communication**
- **Thread-safe I/O (std::cout with mutex)**

## 🧪 Experiments
- Pushed 1000 `Order` objects to a shared queue using 4 producer threads
- Consumed all orders using 2 consumer threads
- Simulated realistic delays with `std::this_thread::sleep_for` to force race conditions
- Identified console output interleaving issues and fixed them using a global `print_mutex`

## 🧰 Tools & Libraries
- C++11 standard features
- `std::thread`, `std::mutex`, `std::condition_variable`

## 🖥️ How to Build and Run
Use the provided `build.sh`:
```bash
chmod +x build.sh
./build.sh
