#pragma once
#include <chrono>
#include <string>
#include <iostream>

class Profiler {
public:
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    long stop(const std::string& msg) {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
        std::cout << "[PROFILE] " << msg << " took " << duration << " ns\n";
        return duration;
    }

private:
    std::chrono::high_resolution_clock::time_point start_time;
};
