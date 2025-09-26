#pragma once

#include "HashLFU.decl.hpp"
#include <cmath>
#include <thread>

template <typename KeyType, typename ValueType>
HashLFUCache<KeyType, ValueType>::HashLFUCache(int capacity, int maxAverageFreq, int slice_count)
    : capacity_(capacity)
    , sliceCount_(slice_count > 0 ? slice_count : std::thread::hardware_concurrency())

{
    int slice_size = std::ceil(static_cast<double>(capacity_) / sliceCount_);
    slicedCaches_.reserve(sliceCount_);
    for (int i = 0; i < sliceCount_; i++)
        slicedCaches_.emplace_back(
            std::make_unique<LFUCache<KeyType, ValueType>>(slice_size, maxAverageFreq));
}

template <typename KeyType, typename ValueType>
bool HashLFUCache<KeyType, ValueType>::get(KeyType key, ValueType& result)
{
    int slice_index = getHash(key) % sliceCount_;
    return slicedCaches_[slice_index]->get(key, result);
}

template <typename KeyType, typename ValueType>
ValueType HashLFUCache<KeyType, ValueType>::get(KeyType key)
{
    int slice_index = getHash(key) % sliceCount_;
    return slicedCaches_[slice_index]->get(key);
}

template <typename KeyType, typename ValueType>
void HashLFUCache<KeyType, ValueType>::put(KeyType key, ValueType value)
{
    int slice_index = getHash(key) % sliceCount_;
    slicedCaches_[slice_index]->put(key, value);
}

template <typename KeyType, typename ValueType>
int HashLFUCache<KeyType, ValueType>::getHash(KeyType key)
{
    std::hash<KeyType> hashFunc;
    return hashFunc(key);
}