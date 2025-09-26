#pragma once

#include "../common/BaseCache.hpp"
#include "../lfu/lfu.hpp"
#include "../lru/lru.hpp"
#include "ArcLfuPart/ArcLfuPart.hpp"
#include "ArcLruPart/ArcLruPart.hpp"
#include <unordered_map>

template <typename KeyType, typename ValueType>
class ARCCache : public BaseCache<KeyType, ValueType>
{
    std::shared_ptr<LRUCache<KeyType, ValueType>> lruGhost_; // LRU最近淘汰缓存
    std::shared_ptr<LRUCache<KeyType, ValueType>> lfuGhost_; // LFU最近淘汰缓存

    std::unique_ptr<ArcLruPart<KeyType, ValueType>> lruPart_; // LRU部分缓存
    std::unique_ptr<ArcLfuPart<KeyType, ValueType>> lfuPart_; // LFU部分缓存

  public:
    /**
     * @brief ARC Cache 构造函数
     * @param capacity LRU 部分和 LFU 部分缓存的容量
     * @param maxAverageFreq LFU 部分的最大平均频率
     */
    ARCCache(int capacity, int maxAverageFreq);

    bool      get(KeyType key, ValueType& result) override;
    ValueType get(KeyType key) override;
    void      put(KeyType key, ValueType value) override;
};
