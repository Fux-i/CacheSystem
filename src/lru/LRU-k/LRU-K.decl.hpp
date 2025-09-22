#pragma once

#include "../LRU/LRU.hpp"
#include <memory>
#include <mutex>

template <typename KeyType, typename ValueType>
class LRUKCache : public LRUCache<KeyType, ValueType>
{
    using mapType = std::unordered_map<KeyType, ValueType>;

    size_t  k_;           // 访问多少次进入缓存
    mapType history_map_; // 存储访问次数未达到k次的数据
    std::unique_ptr<LRUCache<KeyType, size_t>> history_cache_; // 访问数据历史记录

    mutable std::mutex history_mutex_; // 保护history_map_和history_cache_的互斥锁

  public:
    LRUKCache(size_t k, size_t capacity, size_t history_capacity);

    bool      get(KeyType key, ValueType& result) override;
    ValueType get(KeyType key) override;
    void      put(KeyType key, ValueType value) override;
};
