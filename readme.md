# Cache System

——一个 C++缓存系统

实现了以下几种缓存淘汰策略

- LRU（Least Recently Used）
  - 基础 LRU
  - LRU-K：访问 K 次才进入缓存，避免热数据被大量新的冷数据淘汰
  - HashLRU：分片 LRU，提高并发性能
- LFU（Least Frequently Used）
  - 基础 LFU：加入最大平均频次，避免旧热点数据无法清除
  - HashLFU：分片 LFU，提高并发性能
- ARC（Adaptive Replacement Cache）
  - 基础 ARC：一种自适应缓存替换策略，结合 LRU 和 LFU 的优点，在不同情况下自动调整 LRU 和 LFU 的容量
