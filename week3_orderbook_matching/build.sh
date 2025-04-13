// ===== build.sh =====
#!/bin/bash
g++ main.cpp -std=c++11 -pthread -o orderbook

if [ $? -eq 0 ]; then
    echo "✅ Build success!"
    ./orderbook
else
    echo "❌ Build failed."
fi