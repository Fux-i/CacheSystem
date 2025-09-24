#pragma once

#include "../../BaseCache.hpp"
#include "../LFU/LFU.hpp"
#include <memory>
#include <vector>

template <typename KeyType, typename ValueType>
class HashLFUCache : public BaseCache<KeyType, ValueType>
{
    int                                                        capacity_;      // 总容量
    int                                                        slice_count_;   // 分片数量
    std::vector<std::unique_ptr<LFUCache<KeyType, ValueType>>> sliced_caches_; // 分片缓存

  public:
    HashLFUCache(int capacity, int max_average_freq, int slice_count);

    bool      get(KeyType key, ValueType& result) override;
    ValueType get(KeyType key) override;
    void      put(KeyType key, ValueType value) override;

  private:
    int getHash(KeyType key);
};