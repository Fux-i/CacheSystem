#pragma once

#include "HashLRU.decl.hpp"
#include <functional>

template <typename KeyType, typename ValueType>
HashLRUCache<KeyType, ValueType>::HashLRUCache(int capacity, int slice_count)
    : capacity_(capacity)
    , sliceCount_(slice_count > 0 ? slice_count : std::thread::hardware_concurrency())
{
    int slice_size = std::ceil(static_cast<double>(capacity_) / sliceCount_);
    slicedCaches_.reserve(sliceCount_);
    for (int i = 0; i < sliceCount_; i++)
        slicedCaches_.emplace_back(std::make_unique<LRUCache<KeyType, ValueType>>(slice_size));
}

template <typename KeyType, typename ValueType>
bool HashLRUCache<KeyType, ValueType>::get(KeyType key, ValueType& result)
{
    int slice_index = getHash(key) % sliceCount_;
    return slicedCaches_[slice_index]->get(key, result);
}

template <typename KeyType, typename ValueType>
ValueType HashLRUCache<KeyType, ValueType>::get(KeyType key)
{
    int slice_index = getHash(key) % sliceCount_;
    return slicedCaches_[slice_index]->get(key);
}

template <typename KeyType, typename ValueType>
void HashLRUCache<KeyType, ValueType>::put(KeyType key, ValueType value)
{
    int slice_index = getHash(key) % sliceCount_;
    slicedCaches_[slice_index]->put(key, value);
}

template <typename KeyType, typename ValueType>
int HashLRUCache<KeyType, ValueType>::getHash(KeyType key)
{
    std::hash<KeyType> hash_func;
    return hash_func(key);
}
