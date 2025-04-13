# Week 3: OrderBook Matching Engine with Partial Fill Support

## ✅ Goal
This week focuses on implementing the core matching logic of a simplified order book from scratch. The goal is to simulate how real-world limit orders are handled, matched, and partially filled, while building the foundation for cancelation and modification logic in the next phase.

## 📦 Files
- `main.cpp`: Spawns a producer thread to submit orders and a consumer thread to process them using the order book.
- `order.hpp`: Defines the `Order` structure with price, quantity, side, timestamp, and a canceled flag.
- `order_book.hpp`: Implements the core `OrderBook` logic using price-time priority for matching.
- `thread_safe_queue.hpp`: A reusable thread-safe queue implementation.
- `build.sh`: Compilation and execution script.

## 🧠 Concepts Practiced
- **Price-time priority queue using `std::priority_queue`**
- **Order matching with partial fill support**
- **Shared pointer (`std::shared_ptr`) for dynamic order tracking**
- **Atomic timestamp counter for FIFO ordering**
- **Multi-threaded coordination using thread-safe queues**

## 🔁 Matching Logic Implemented
- Buy orders are stored in a max-heap, sell orders in a min-heap
- Orders are matched when `buy_price >= sell_price`
- Matches consume quantity from both sides
- Orders with remaining quantity are kept in the queue
- Canceled orders are skipped

## 📘 Takeaways
- Partial fill support requires real-time updates to quantity fields
- Tracking orders via shared pointers enables in-place modification
- Proper synchronization and separation of logic is critical for scalable matching

## 🔜 Next Step
In Week 4, we will introduce an `order_id → pointer` mapping to support cancel and modify commands efficiently and extend the order book interface.

