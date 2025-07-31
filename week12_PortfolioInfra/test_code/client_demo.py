import zmq
import time
import random

ctx = zmq.Context()
s = ctx.socket(zmq.REQ)
s.connect("tcp://localhost:5555")

symbols = ["AAPL", "MSFT"]
order_id = 1000

for i in range(10):
    symbol = random.choice(symbols)
    is_buy = random.choice([1, 0])
    price = round(150 + random.random() * 5, 2)
    qty = random.choice([10, 20, 50, 100])
    order_type = "LIMIT"  # 你也可以用 "MARKET" 測試

    msg = f"ORDER|{order_id}|{symbol}|{is_buy}|{price}|{qty}|{order_type}"
    print(f"Send: {msg}")
    s.send_string(msg)
    print("Gateway reply:", s.recv_string())
    order_id += 1
    time.sleep(0.1)
