#include <iostream>
#include <string>
#include "liquibook/src/book/depth_order_book.h"
#include "liquibook/src/book/types.h"
#include "liquibook/src/book/order_listener.h"
#include "liquibook/src/book/trade_listener.h"

using namespace liquibook::book;
using namespace std;

// =====================
// ✅ 自訂 Order 類別
// =====================
class MyOrder {
public:
    MyOrder(const string& id, bool is_buy, Price price, Quantity quantity)
        : id_(id), is_buy_(is_buy), price_(price), quantity_(quantity), filled_qty_(0) {}

    bool is_buy() const { return is_buy_; }
    Price price() const { return price_; }
    Quantity order_qty() const { return quantity_; }
    Quantity filled_qty() const { return filled_qty_; }
    bool is_limit() const { return true; }
    Price stop_price() const { return 0; }
    string id() const { return id_; }

    void fill(Quantity qty) { filled_qty_ += qty; }

private:
    string id_;
    bool is_buy_;
    Price price_;
    Quantity quantity_;
    Quantity filled_qty_;
};

// ===============================
// ✅ Listener 實作（Order + Trade）
// ===============================
// typedef MyOrder* OrderPtr;
using OrderPtr = MyOrder*;
using BaseOrderBook = OrderBook<OrderPtr>;           // for TradeListener
using FullOrderBook = DepthOrderBook<OrderPtr>;      // actual book type

class MyListener :
    public OrderListener<OrderPtr>,
    public TradeListener<BaseOrderBook> {
public:
    void on_accept(const OrderPtr& order) override {
        cout << "[ACCEPT] " << order->id()
             << " (" << (order->is_buy() ? "BUY" : "SELL")
             << " @" << order->price() / 100.0
             << " x " << order->order_qty() << ")" << endl;
    }

    void on_fill(const OrderPtr& order,
                 const OrderPtr& matched_order,
                 Quantity qty,
                 Price price) override {
        order->fill(qty);
        cout << "[FILL] " << order->id()
             << " filled " << qty << " @ $" << price / 100.0
             << " (" << order->filled_qty() << "/" << order->order_qty() << ")"
             << endl;
    }

    void on_cancel(const OrderPtr& order) override {
        cout << "[CANCEL] " << order->id() << " canceled." << endl;
    }

    void on_trade(const BaseOrderBook* book,
                  Quantity qty,
                  Price price) override {
        cout << "[TRADE] total qty: " << qty << " @ $" << price / 100.0 << endl;
    }

    // ✅ 空實作：滿足純虛擬函式
    void on_reject(const OrderPtr& order, const char* reason) override {}
    void on_cancel_reject(const OrderPtr& order, const char* reason) override {}
    void on_replace(const OrderPtr& order, const int64_t& size_delta, Price new_price) override {}
    void on_replace_reject(const OrderPtr& order, const char* reason) override {}
};

// =====================
// ✅ Main 程式
// =====================
int main() {
    FullOrderBook book;
    MyListener listener;

    book.set_order_listener(&listener);
    book.set_trade_listener(&listener);

    MyOrder* buy = new MyOrder("BUY1", true, 10000, 10);     // $100.00
    MyOrder* sell = new MyOrder("SELL1", false, 10000, 10);  // $100.00

    book.add(buy);
    book.add(sell);

    cout << "✅ 撮合完成。" << endl;

    delete buy;
    delete sell;
    return 0;
}

