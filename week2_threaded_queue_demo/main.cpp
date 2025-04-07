#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "thread_safe_queue.hpp"

using namespace std;

//per order look like
//I can write another class call order
struct Order {
    string id;
    bool is_buy;
    double price;
    int quantity;

    Order(string i, bool b, double p, int q)
        : id(i), is_buy(b), price(p), quantity(q) {}

};

void producer(ThreadSafeQueue<Order>& queue) {
    for (int i = 0; i < 5; ++i) {
        Order o("ORD" + to_string(i), i % 2 == 0, 100.0 + i, 10);
        cout << "[Producer] New Order: " << o.id << endl;
        queue.push(o);
        this_thread::sleep_for(chrono::milliseconds(500));  //simulate every 0.5 sec comes a data
    }
}

void consumer(ThreadSafeQueue<Order>& queue) {
    for (int i = 0; i < 5; ++i) {
        Order o = queue.pop();
        cout << "[Consumer] Processing Order: " << o.id << endl;
        this_thread::sleep_for(chrono::milliseconds(300)); //simulate deal with each data for 0.3 sec
    }
}

int main() {

    //Declare a ThreadSafeQueue class Define Type is  struct Order
    ThreadSafeQueue<Order> queue;

    //construct a thread, passing funtion and parameter with reference
    thread t1(producer, ref(queue));
    thread t2(consumer, ref(queue));
    
    t1.join();  //wait for producer finish
    t2.join();  //wait for consumer finish

    cout << "✅ All orders processed." << endl;
    return 0;
}
