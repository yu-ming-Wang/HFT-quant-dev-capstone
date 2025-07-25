#!/bin/bash
g++ main.cpp -std=c++17 -o orderbook
if [ $? -eq 0 ]; then
    echo "✅ Build success!"
    ./orderbook
else
    echo "❌ Build failed."
fi
