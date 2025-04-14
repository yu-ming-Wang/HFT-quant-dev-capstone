# Week 4: Order Management – Cancel & Modify Support

## ✅ Goal
Enhance the core matching engine from Week 3 by supporting **order cancellation** and **modification**, enabling a realistic trading system that can respond dynamically to user actions.

---

## 📁 File Overview

| File                     | Description                                      |
|--------------------------|--------------------------------------------------|
| `order.hpp`              | Defines the `Order` struct with new fields like `canceled` |
| `order_book.hpp`         | Implements order book logic including `cancel_order()` and `modify_order()` |
| `thread_safe_queue.hpp`  | Thread-safe FIFO queue (not used in Week 4 demo but reusable) |
| `main.cpp`               | Test suite for adding, canceling, and modifying orders |
| `build.sh`               | Shell script to compile and run the project     |

---

## 🧠 Key Concepts Implemented

- **Order Cancellation**:  
  Orders can be flagged as `canceled`, and the matching engine will ignore them.

- **Order Modification**:  
  If an order has not been fully matched or canceled, it is removed and replaced with a new one (same ID, updated price/qty).

- **Protection Against Invalid Actions**:  
  - Modification of filled/canceled orders is rejected with informative logs.
  - Cancelled orders are skipped during matching.

---

## 🔁 Matching Behavior

- Uses **price-time priority** to determine match sequence.
- Both BUY (max-heap) and SELL (min-heap) queues ignore canceled orders.
- Quantity is reduced upon match; unmatched orders remain in the book.

---

## 🧪 Example Actions from `main.cpp`

```cpp
book.cancel_order("ORD3");               // Cancel order
book.modify_order("ORD1", 99.0, 8);      // Modify order after partial match
book.modify_order("ORD2", 98.5, 5);      // ❌ Already matched → Modify fails
book.cancel_order("ORD5");              // Cancel before match
book.modify_order("ORD6", 101.5, 5);     // ✅ Successfully modified
