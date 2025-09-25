#pragma once

#include "../../common/BaseCache.hpp"
#include "../LFU/LFU.hpp"
#include <memory>
#include <vector>

template <typename KeyType, typename ValueType>
class HashLFUCache : public BaseCache<KeyType, ValueType>
{
    int                                                        capacity_;     // 总容量
    int                                                        sliceCount_;   // 分片数量
    std::vector<std::unique_ptr<LFUCache<KeyType, ValueType>>> slicedCaches_; // 分片缓存

  public:
    HashLFUCache(int capacity, int max_average_freq, int slice_count);

    bool      get(KeyType key, ValueType& result) override;
    ValueType get(KeyType key) override;
    void      put(KeyType key, ValueType value) override;

  private:
    int getHash(KeyType key);
};