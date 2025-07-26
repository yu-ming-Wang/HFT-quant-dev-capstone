#!/bin/bash
g++ main.cpp -std=c++17 -o backtest
if [ $? -eq 0 ]; then
    echo "✅ Build success!"
    ./backtest
else
    echo "❌ Build failed."
fi
