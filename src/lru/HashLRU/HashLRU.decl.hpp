#pragma once

#include "../../common/BaseCache.hpp"
#include "../LRU/LRU.hpp"
#include <cmath>
#include <memory>
#include <thread>
#include <vector>

template <typename KeyType, typename ValueType>
class HashLRUCache : public BaseCache<KeyType, ValueType>
{
    int                                                        capacity_;     // 总容量
    int                                                        sliceCount_;   // 分片数量
    std::vector<std::unique_ptr<LRUCache<KeyType, ValueType>>> slicedCaches_; // 分片缓存

  public:
    HashLRUCache(int capacity, int slice_count);

    bool      get(KeyType key, ValueType& result) override;
    ValueType get(KeyType key) override;
    void      put(KeyType key, ValueType value) override;

  private:
    int getHash(KeyType key);
};
