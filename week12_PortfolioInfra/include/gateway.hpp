#pragma once
#include <zmq.hpp>
#include <thread>
#include <atomic>
#include <vector>
#include "engine_manager.hpp"
#include "order.hpp"

class Gateway {
public:
    Gateway(const std::string& bind_addr, EngineManager* em, int num_workers = 4);
    void start();
    void stop();

private:
    void frontend_loop();   // ROUTER (frontend)
    void worker_loop();     // DEALER (workers)
    Order parse_order(const std::string& data);

    zmq::context_t ctx;
    zmq::socket_t frontend; // ROUTER
    EngineManager* engine_mgr;
    std::atomic<bool> running;
    int worker_count;
    std::vector<std::thread> workers;
};
