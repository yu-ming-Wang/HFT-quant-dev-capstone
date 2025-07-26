import random
import pandas as pd
from datetime import datetime, timedelta

# 參數
symbol = "AAPL"
start_time = datetime(2023, 7, 20, 9, 30, 0)
base_price = 100.0
n = 1000

rows = []
prices = []

for i in range(n):
    timestamp = (start_time + timedelta(seconds=i)).strftime("%Y%m%dT%H%M%S")
    last_price = round(base_price + random.gauss(0, 1), 2)
    high = round(last_price + random.uniform(0, 1), 2)
    low = round(last_price - random.uniform(0, 1), 2)
    prices.append(last_price)
    avg = round(sum(prices[-20:]) / min(len(prices), 20), 2)
    volume = random.randint(4000, 7000)
    rows.append([timestamp, symbol, last_price, high, low, avg, volume])

# 存成 DataFrame
df = pd.DataFrame(rows, columns=["timestamp", "symbol", "last_price", "high", "low", "avg", "volume"])

# 寫出 CSV
df.to_csv("AAPL_data.csv", index=False)
