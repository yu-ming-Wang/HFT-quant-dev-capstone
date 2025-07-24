#!/bin/bash
g++ stress_test.cpp -std=c++17 -pthread -o stress_test
if [ $? -eq 0 ]; then
    echo "✅ Build success!"
    ./stress_test
else
    echo "❌ Build failed."
fi
