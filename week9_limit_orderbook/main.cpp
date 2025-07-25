#include "orderbook.hpp"
#include <memory>
#include <vector>
#include <string>

int main() {
    // Define three symbols for the simulation
    std::vector<std::string> symbols = {"AAPL", "GOOG", "TSLA"};
    std::vector<std::unique_ptr<OrderBook>> books;

    // Create an OrderBook for each symbol
    for (const auto& sym : symbols)
        books.push_back(std::make_unique<OrderBook>(sym));

    // For each symbol, add 5 buy and 5 sell limit orders
    for (int i = 0; i < symbols.size(); ++i) {
        double base = 100 + i * 10; // Offset base price for each symbol
        for (int k = 0; k < 5; ++k) {
            // Place buy limit orders (from low to high price)
            books[i]->add_order(true, base + k * 0.5, 3 + k, OrderType::LIMIT);
            // Place sell limit orders (from higher price levels)
            books[i]->add_order(false, base + 3 + k * 0.5, 2 + k, OrderType::LIMIT);
        }
    }

    // Print initial order book snapshots for all symbols
    std::cout << "=== Initial OrderBook Snapshots ===\n";
    for (const auto& b : books)
        b->print_book();

    // For each symbol, simulate a market buy and market sell order
    for (int i = 0; i < symbols.size(); ++i) {
        std::cout << "\n[MARKET] " << symbols[i] << " market BUY 7\n";
        books[i]->add_order(true, 0, 7, OrderType::MARKET);

        std::cout << "\n[MARKET] " << symbols[i] << " market SELL 8\n";
        books[i]->add_order(false, 0, 8, OrderType::MARKET);
    }

    // Print the order book snapshots again after market orders
    std::cout << "\n=== OrderBook Snapshots After Market Orders ===\n";
    for (const auto& b : books)
        b->print_book();

    return 0;
}
