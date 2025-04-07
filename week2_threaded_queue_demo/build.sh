#!/bin/bash

# 編譯 main.cpp 成 main
g++ main.cpp -std=c++11 -pthread -o main
if [ $? -eq 0 ]; then
    echo "✅ Built main"
else
    echo "❌ Failed to build main"
fi

# 編譯 multi_thread_order.cpp 成 multi_thread_order
g++ multi_thread_order.cpp -std=c++11 -pthread -o multi_thread_order
if [ $? -eq 0 ]; then
    echo "✅ Built multi_thread_order"
else
    echo "❌ Failed to build multi_thread_order"
fi
