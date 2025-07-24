#!/bin/bash
g++ exchange_runner.cpp -std=c++17 -pthread -o exchange_runner

if [ $? -eq 0 ]; then
    echo "✅ Build success!"
    ./exchange_runner
else
    echo "❌ Build failed."
fi
