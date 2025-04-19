// ===== build.sh =====
#!/bin/bash
g++ main.cpp -std=c++11 -pthread -o exchange

if [ $? -eq 0 ]; then
    echo "✅ Build success!"
    ./exchange
else
    echo "❌ Build failed."
fi
