```
NOw:
       [ Client1 ]   [ Client2 ]   [ ClientN ]                        [ Feed AAPL ]   [ Feed MSFT ]   [ Feed GOOG ]
              │            │            │                                     │             │             │
              └───────┬────┴───────┬────┘                                     └────┬────────┴─────┬───────┘
                      │            │                                               │              │
             [ Gateway (ZeroMQ) ]  │                                               │              │
                      │            │                                               │              │
                      └────────────┴───────────────────────────────────────────────┴──────────────┘
                                                     │
                              ┌──────────────────────┴──────────────────────┐
                              │  LockFree Order Queue (RingBuffer<Order>)   │
                              └──────────────────────┬──────────────────────┘
                                                     │
                                       [ Engine Routing (submit_order) ]
                                                     │
                      ┌──────────────────────────────┴───────────────────────────────┐
                      │   EngineManager (per-symbol OrderBookOptimized)              │
                      │     - submit_order() → route to correct OB                   │
                      │     - match_all() → match AAPL / MSFT / GOOG                 │
                      └──────────────────────────────┬───────────────────────────────┘
                                                     │
              ┌──────────────────────────────────────┼──────────────────────────────────────┐
              │                                      │                                      │
      [ TradeLogger.csv ]                  [ PortfolioManager ]                  [ LatencyMonitor.csv ]
```


---

```
Future:
      [ Real Exchange Network ]                                    [ Client1 ] [ Client2 ] [ ClientN ]
                │                                                            │       │       │
     (UDP/TCP/Multicast Packets)                                             └──┬────┴───┬───┘
                │                                                               │        │
                ▼                                                               ▼        │
 [ NIC (DPDK / Kernel Bypass) ]                                         [ Gateway (ZeroMQ/ROUTER) ]
                │                                                               │
                ▼                                                               │
         [ Packet Capture ]                                                     │
                │                                                               │
                ▼                                                               │
    [ Feed Handler / Protocol Parser ]                                          │
    - 解封包 ITCH / OUCH / FIX / CME MDP                                         │
    - 時間同步 (PTP)                                                             │
    - 驗證序列 / Checksum                                                        │
    - 轉換成內部 Tick/Book Update                                                │
                │                                                               │
                └────────────────────────────────────┐                          │
                                                     │                          │
                                                     ▼                          ▼
                                      ┌──────────────────────────────────────────┐
                                      │   LockFree Order Queue (RingBuffer)      │
                                      └──────────────────────────────────────────┘
                                                     │
                                         [ Engine Routing (submit_order) ]
                                                     │
                            ┌─────────────────────────┴─────────────────────────────┐
                            │ EngineManager (OrderBook per Symbol)                  │
                            │   - submit_order() → route to OB                      │
                            │   - match_all() → 撮合成交                             │
                            └─────────────────────────┬─────────────────────────────┘
                                                     │
          ┌──────────────────────────────────────────┼─────────────────────────────────────────┐
          │                                          │                                         │
 [ TradeLogger.csv ]                       [ PortfolioManager ]                     [ LatencyMonitor.csv ]


```


```
MINI Exchange

      [ Real Exchange Network ]
                │
     (UDP/TCP/Multicast Packets)
                │  UDP / TCP (ITCH / OUCH / FIX)
                ▼
 [ Packet Capture + Feed Handler ]
 - 低延遲解封包 + 標準化行情
                │
                ▼
 ┌───────────────────────────────────────────────────────────────────────────────┐
 │                      Web Frontend (Dashboard / Trader UI)                     │
 │                                                                               │
 │   - **REST API / WebSocket** → 發送訂單 → Gateway                              │ 
 │   - **WebSocket / SSE** ← 訂閱 Market Feed (行情流)                            │
 │   - 顯示 OrderBook / PnL / Latency                                            │
 │   - 從 **PostgreSQL** 查詢歷史數據                                             │
 │                                                                               │
 │   [ Market Feed Threads ] →→→→→ (WebSocket Stream) →→→→→ **UI 即時更新**       │
 │                                                                               │
 │                                    [ Client1 ] [ Client2 ] [ AlgoBotN ]       │
 │                                              │       │         │              │
 │                                              └───────┴─────────┘              │
 │                                                      │                        │
 │                                  (ZeroMQ REQ/REP 或 ROUTER/DEALER)            │
 │                                                      ▼                        │
 │                                             [ Gateway (ZeroMQ) ]              │
 └───────────────────────────────────────────────│───────────────────────────────┘
                                                 ▼
                               ┌───────────────────────────────────────────────┐
                               │   LockFree Order Queue (RingBuffer<Order>)   │
                               └──────────────────────┬────────────────────────┘
                                                      │
                                          [ Engine Routing (submit_order) ]
                                                      │
                          ┌───────────────────────────┴─────────────────────────────┐
                          │ EngineManager (OrderBook per Symbol)                   │
                          │   - match_all() → 撮合成交                             │
                          └───────────────────────────┬─────────────────────────────┘
                                                      │
          ┌───────────────────────────────────────────┼─────────────────────────────────────────────┐
          │                                           │                                             │
 [ TradeLogger.csv ]                         [ PortfolioManager ]                        [ LatencyMonitor.csv ]
          │                                           │                                             │
          └─────────────┬─────────────────────────────┘                                             │
                        ▼                                                                           ▼
           ┌───────────────────────────┐                                        ┌─────────────────────────────┐
           │   Redis (實時 OrderBook)   │  <─── **WebSocket/REST → Web Frontend** │  Analytics DB (PostgreSQL) │
           └───────────────────────────┘                                        └─────────────────────────────┘



```




```
             ┌─────────────────────────────────────────────┐
             │              Clients (DEALER)               │
             │   AlgoBot1     AlgoBot2     WebUI Trader    │
             └───────┬──────────────┬──────────────┬───────┘
                     │              │              │
                     ▼              ▼              ▼
             ┌─────────────────────────────────────────────┐
             │        ROUTER Frontend (Gateway)            │
             │  - Receives all client messages             │
             │  - Does not handle business logic           │
             │    (only forwards messages)                 │
             └───────────────┬─────────────────────────────┘
                             │
                             ▼
                 ┌─────────────────────────────────────┐
                 │      ZeroMQ Proxy                   │
                 │  - Auto routes Client → Worker      │
                 │  - Auto routes Worker → Client      │
                 │  - Built-in lock-free message queue │
                 └───────────────┬─────────────────────┘
                                 │ (inproc)
         ┌───────────────────────┼──────────────────────────┐
         │                       │                          │
         ▼                       ▼                          ▼
┌───────────────────┐   ┌───────────────────┐   ┌───────────────────┐
│ Worker DEALER #1  │   │ Worker DEALER #2  │   │ Worker DEALER #3  │
│ - Parse order     │   │ - Risk checks     │   │ - Order handling  │
│ - Push order_queue│   │ - Push order_queue│   │ - Push order_queue│
│ - Send ACK        │   │ - Send ACK        │   │ - Send ACK        │
└───────────────────┘   └───────────────────┘   └───────────────────┘

```


```
             ┌─────────────────────────────────────────────┐
             │              Clients (DEALER)               │
             │   AlgoBot1     AlgoBot2     WebUI Trader    │
             └───────┬──────────────┬──────────────┬───────┘
                     │              │              │
       tcp://<my_server_ip>:8787    │              │
                     ▼              ▼              ▼

        ┌─────────────────────────────────────────────────────┐
        │               ZeroMQ Proxy (Gateway Core)           │
        │                                                     │
        │   ┌─────────────────────────────────────────────┐   │
        │   │      ROUTER Frontend (Gateway)              │   │
        │   │  - Binds tcp://*:8787                       │   │
        │   │  - Receives all client messages             │   │
        │   │  - Forwards to Backend DEALER               │   │
        │   └───────────────────┬─────────────────────────┘   │
        │                       │                             │
        │                       ▼                             │
        │   ┌─────────────────────────────────────────────┐   │
        │   │   Backend DEALER (Proxy Backend Endpoint)   │   │
        │   │  - Binds inproc://yuming_slave              │   │
        │   │  - Distributes messages to Worker DEALERs   │   │
        │   │  - Handles ACK routing back to clients      │   │
        │   └───────────────────┬─────────────────────────┘   │
        └───────────────────────┼─────────────────────────────┘
                                │
                                │  (Workers connect here)
                                ▼
         ┌──────────────────────┼──────────────────────────┐
         │                      │                          │
         ▼                      ▼                          ▼
┌───────────────────┐   ┌───────────────────┐   ┌───────────────────┐
│ Worker DEALER #1  │   │ Worker DEALER #2  │   │ Worker DEALER #3  │
│ - Connects to     │   │ - Connects to     │   │ - Connects to     │
│   inproc://yuming_slave │   inproc://yuming_slave │   inproc://yuming_slave │
│ - Parse order     │   │ - Risk checks     │   │ - Order handling  │
│ - Push order_queue│   │ - Push order_queue│   │ - Push order_queue│
│ - Send ACK        │   │ - Send ACK        │   │ - Send ACK        │
└───────────────────┘   └───────────────────┘   └───────────────────┘

```