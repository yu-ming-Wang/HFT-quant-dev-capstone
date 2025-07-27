#!/bin/bash
set -e

g++ main.cpp -std=c++17 -O3 -march=native -pthread -o week11_low_latency

echo "✅ Build success!"
./week11_low_latency
