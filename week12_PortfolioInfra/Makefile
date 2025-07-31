CXX = g++
CXXFLAGS = -std=c++17 -O3 -pthread -Wall

SRC = src/main.cpp src/gateway.cpp src/market_feed.cpp src/engine_manager.cpp src/portfolio_manager.cpp src/latency_monitor.cpp

INCLUDE = -Iinclude -I/usr/local/include

TARGET = week12_portfolioinfra

# 預設編譯
all: $(TARGET)

# 編譯規則
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(INCLUDE) -lzmq -o $(TARGET)

# 執行規則：確保 results 目錄存在並執行
run: $(TARGET)
	mkdir -p results
	./$(TARGET)

# 清除
clean:
	rm -f $(TARGET)

.PHONY: all clean run
