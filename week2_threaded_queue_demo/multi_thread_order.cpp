#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include "thread_safe_queue.hpp"

using namespace std;
//add a print mutex lock to avoid cout crash
std::mutex print_mutex;


// ====== Order class ======
struct Order {
    string id;
    bool is_buy;
    double price;
    int quantity;

    Order(string i, bool b, double p, int q)
        : id(i), is_buy(b), price(p), quantity(q) {}
};

// ====== Producer thread ======
void producer_batch(ThreadSafeQueue<Order>& queue, vector<Order>& orders, int start, int end) {
    for (int i = start; i < end; ++i) {
        this_thread::sleep_for(chrono::milliseconds(1));  // simulate delay
        queue.push(orders[i]);
        {
            std::lock_guard<std::mutex> lock(print_mutex);
            cout << "[Producer " << this_thread::get_id() << "] Pushed " << orders[i].id << endl;
        }
    }
}

// ====== Consumer thread ======
void consumer_loop(ThreadSafeQueue<Order>& queue, int expected) {
    for (int i = 0; i < expected; ++i) {
        Order o = queue.pop();
        {
            std::lock_guard<std::mutex> lock(print_mutex);
            cout << "[Consumer " << this_thread::get_id() << "] Got " << o.id << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(2));  // simulate processing
    }
}

// ====== Main ======
int main() {
    ThreadSafeQueue<Order> queue;
    vector<Order> all_orders;
  
    // Create 1000 orders
    for (int i = 0; i < 1000; ++i) {
        //for qulity and efficient, we dont use constructer to construct a tmp class and push into vector
        all_orders.emplace_back("ORD" + to_string(i), i % 2 == 0, 100.0 + i, 10);
    }

    int num_producers = 4;
    int num_consumers = 2;
    int orders_per_thread = 1000 / num_producers;

    vector<thread> threads;

    // Launch producer threads
    for (int i = 0; i < num_producers; ++i) {
        int start = i * orders_per_thread;
        int end = (i == num_producers - 1) ? 1000 : start + orders_per_thread;
        
        //thread pass a code function and parameter(both use reference and copy) 
        threads.emplace_back(producer_batch, ref(queue), ref(all_orders), start, end);
    }

    // Launch consumer threads
    for (int i = 0; i < num_consumers; ++i) {
        threads.emplace_back(consumer_loop, ref(queue), 1000 / num_consumers);
    }

    // Wait for all threads to finish
    for (auto& t : threads)
        t.join();

    cout << "\n✅ All orders processed." << endl;
    return 0;
}
