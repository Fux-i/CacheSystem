#pragma once

#include "../../BaseCache.hpp"
#include "../LRU/LRU.hpp"
#include <cmath>
#include <memory>
#include <thread>
#include <vector>

template <typename KeyType, typename ValueType>
class HashLRUCache : public BaseCache<KeyType, ValueType>
{
    size_t                                                     capacity_;      // 总容量
    size_t                                                     slice_count_;   // 分片数量
    std::vector<std::unique_ptr<LRUCache<KeyType, ValueType>>> sliced_caches_; // 分片缓存

  public:
    HashLRUCache(size_t capacity, size_t slice_count);

    bool      get(KeyType key, ValueType& result) override;
    ValueType get(KeyType key) override;
    void      put(KeyType key, ValueType value) override;

  private:
    size_t get_hash(KeyType key);
};
