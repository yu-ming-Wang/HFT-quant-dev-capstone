import os
import pandas as pd
import matplotlib.pyplot as plt

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
CSV_PATH = os.path.join(BASE_DIR, "../results/latency/latency.csv")
OUT_DIR = os.path.join(BASE_DIR, "../results/latency")
OUT_PATH = os.path.join(OUT_DIR, "latency_histogram.png")

# ✅ 自動建立資料夾
os.makedirs(OUT_DIR, exist_ok=True)

df = pd.read_csv(CSV_PATH, names=["latency_ns"], header=0 if open(CSV_PATH).readline().startswith("latency_ns") else None)

print("平均延遲 (ns):", df['latency_ns'].mean())
print("P50:", df['latency_ns'].median())
print("P90:", df['latency_ns'].quantile(0.9))
print("P99:", df['latency_ns'].quantile(0.99))
print("最大:", df['latency_ns'].max())

plt.hist(df['latency_ns'], bins=50, edgecolor='black')
plt.xlabel("Latency (ns)")
plt.ylabel("Frequency")
plt.title("Latency Distribution")
plt.savefig(OUT_PATH, dpi=300)   # ✅ 確保路徑存在
plt.show()