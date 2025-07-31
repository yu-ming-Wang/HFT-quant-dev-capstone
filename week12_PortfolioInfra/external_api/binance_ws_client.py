import websocket
import json

def on_message(ws, message):
    print("Market data:", message)

ws = websocket.WebSocketApp(
    "wss://stream.binance.com:9443/ws/btcusdt@trade",
    on_message=on_message
)
ws.run_forever()
