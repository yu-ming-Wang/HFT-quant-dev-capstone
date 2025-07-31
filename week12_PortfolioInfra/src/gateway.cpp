#include "gateway.hpp"
#include "lockfree_ringbuffer.hpp"
#include <iostream>
#include <sstream>
#include <vector>

extern LockFreeRingBuffer<Order, 65536> order_queue;

Gateway::Gateway(const std::string& bind_addr, EngineManager* em, int num_workers)
    : ctx(1), frontend(ctx, ZMQ_ROUTER), engine_mgr(em), running(false), worker_count(num_workers)
{
    // ✅ ROUTER frontend binds TCP for external clients
    frontend.bind(bind_addr);
}

Order Gateway::parse_order(const std::string& data) {
    std::stringstream ss(data);
    std::string token;
    std::vector<std::string> fields;
    while (std::getline(ss, token, '|')) fields.push_back(token);
    if (fields.size() < 7) throw std::runtime_error("Invalid order format");

    return Order(std::stoi(fields[1]), fields[2], std::stoi(fields[3]) != 0,
                 std::stod(fields[4]), std::stoi(fields[5]),
                 (fields[6] == "LIMIT") ? OrderType::LIMIT : OrderType::MARKET);
}

void Gateway::start() {
    running = true;

    // ✅ Backend DEALER endpoint for workers (internal transport)
    std::thread(&Gateway::frontend_loop, this).detach();

    // ✅ Launch worker threads
    for (int i = 0; i < worker_count; i++) {
        workers.emplace_back(&Gateway::worker_loop, this);
    }
}

void Gateway::stop() {
    running = false;
    frontend.close();      // ✅ 主動關閉 ROUTER socket
    ctx.close();           // ✅ 關閉 ZMQ context，釋放 inproc 地址
    for (auto& w : workers) {
        if (w.joinable()) w.join();
    }
}

// ✅ ROUTER Frontend Thread: binds tcp:// and connects to backend DEALER
void Gateway::frontend_loop() {
    zmq::socket_t backend(ctx, ZMQ_DEALER);
    backend.bind("inproc://yuming_slaves");   // ✅ Workers will connect here

    // ✅ ZeroMQ Proxy: handles Client <-> Worker routing automatically
    zmq::proxy(frontend, backend);
}

void Gateway::worker_loop() {
    zmq::socket_t worker(ctx, ZMQ_DEALER);
    worker.connect("inproc://yuming_slaves");

    while (running) {
        zmq::message_t identity;
        zmq::message_t payload;

        // ✅ DEALER 在 proxy 模式下會傳兩個 frame：[identity][payload]
        if (!worker.recv(identity, zmq::recv_flags::none)) continue;
        if (!worker.recv(payload, zmq::recv_flags::none)) continue;

        std::string data(static_cast<char*>(payload.data()), payload.size());

        // ✅ 協議檢查
        if (data.rfind("ORDER|", 0) != 0) {
            std::cerr << "[Worker] Ignored non-order payload: "
                      << data.substr(0, 50) << "..." << std::endl;
            continue;
        }

        try {
            auto order = parse_order(data);
            while (!order_queue.push(order)) {
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
        } catch (const std::exception& e) {
            std::cerr << "[Worker] parse failed: " << e.what()
                      << " | raw data=" << data << std::endl;
        }

        // ✅ 回 ACK（需帶回 identity frame）
        worker.send(identity, zmq::send_flags::sndmore);
        worker.send(zmq::str_buffer("ACK"), zmq::send_flags::none);
    }
}

