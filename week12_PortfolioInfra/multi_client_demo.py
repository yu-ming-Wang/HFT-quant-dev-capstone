import zmq
import threading
import time
import random

ctx = zmq.Context()

def client_thread(name):
    sock = ctx.socket(zmq.DEALER)
    sock.identity = name.encode()
    sock.connect("tcp://localhost:8787")  # ✅ 連接 Engine Gateway

    for i in range(5):  # 每個 client 發送 5 單
        order_id = random.randint(1000, 9999)
        price = round(150 + random.uniform(-2, 2), 2)  # ✅ 讓價格波動 148~152
        qty = random.choice([50, 100, 200])
        is_buy = random.choice([0, 1])  # ✅ 隨機決定買單(1) / 賣單(0)

        msg = f"ORDER|{order_id}|AAPL|{is_buy}|{price}|{qty}|LIMIT"
        print(f"[{name}] sending: {msg}")
        sock.send_string(msg)

        try:
            reply = sock.recv_string(flags=0)
            print(f"[{name}] got reply: {reply}")
        except zmq.ZMQError:
            print(f"[{name}] ❌ No ACK received")

        time.sleep(random.uniform(0.5, 1.5))

    sock.close()

# start 4 client
threads = []
for cid in ["C1", "C2", "C3", "C4"]:
    t = threading.Thread(target=client_thread, args=(cid,))
    t.start()
    threads.append(t)

for t in threads:
    t.join()

ctx.term()
