#pragma once

#include "HashLFU.decl.hpp"
#include <cmath>
#include <thread>

template <typename KeyType, typename ValueType>
HashLFUCache<KeyType, ValueType>::HashLFUCache(int capacity, int max_average_freq, int slice_count)
    : capacity_(capacity)
    , slice_count_(slice_count > 0 ? slice_count : std::thread::hardware_concurrency())

{
    int slice_size = std::ceil(static_cast<double>(capacity_) / slice_count_);
    sliced_caches_.reserve(slice_count_);
    for (int i = 0; i < slice_count_; i++)
        sliced_caches_.emplace_back(
            std::make_unique<LFUCache<KeyType, ValueType>>(slice_size, max_average_freq));
}

template <typename KeyType, typename ValueType>
bool HashLFUCache<KeyType, ValueType>::get(KeyType key, ValueType& result)
{
    int slice_index = getHash(key) % slice_count_;
    return sliced_caches_[slice_index]->get(key, result);
}

template <typename KeyType, typename ValueType>
ValueType HashLFUCache<KeyType, ValueType>::get(KeyType key)
{
    int slice_index = getHash(key) % slice_count_;
    return sliced_caches_[slice_index]->get(key);
}

template <typename KeyType, typename ValueType>
void HashLFUCache<KeyType, ValueType>::put(KeyType key, ValueType value)
{
    int slice_index = getHash(key) % slice_count_;
    sliced_caches_[slice_index]->put(key, value);
}

template <typename KeyType, typename ValueType>
int HashLFUCache<KeyType, ValueType>::getHash(KeyType key)
{
    std::hash<KeyType> hashFunc;
    return hashFunc(key);
}