#!/bin/bash
g++ strategy_demo.cpp -std=c++17 -pthread -o strategy_demo
if [ $? -eq 0 ]; then
    echo "✅ Build success!"
    ./strategy_demo
else
    echo "❌ Build failed."
fi
