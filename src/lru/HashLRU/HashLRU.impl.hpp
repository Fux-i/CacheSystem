#pragma once

#include "HashLRU.decl.hpp"
#include <functional>

template <typename KeyType, typename ValueType>
HashLRUCache<KeyType, ValueType>::HashLRUCache(size_t capacity, size_t slice_count)
    : capacity_(capacity)
    , slice_count_(slice_count > 0 ? slice_count : std::thread::hardware_concurrency())
{
    size_t slice_size = std::ceil(static_cast<double>(capacity_) / slice_count_);
    sliced_caches_.reserve(slice_count_);
    for (size_t i = 0; i < slice_count_; i++)
        sliced_caches_.emplace_back(std::make_unique<LRUCache<KeyType, ValueType>>(slice_size));
}

template <typename KeyType, typename ValueType>
bool HashLRUCache<KeyType, ValueType>::get(KeyType key, ValueType& result)
{
    size_t slice_index = get_hash(key) % slice_count_;
    return sliced_caches_[slice_index]->get(key, result);
}

template <typename KeyType, typename ValueType>
ValueType HashLRUCache<KeyType, ValueType>::get(KeyType key)
{
    size_t slice_index = get_hash(key) % slice_count_;
    return sliced_caches_[slice_index]->get(key);
}

template <typename KeyType, typename ValueType>
void HashLRUCache<KeyType, ValueType>::put(KeyType key, ValueType value)
{
    size_t slice_index = get_hash(key) % slice_count_;
    sliced_caches_[slice_index]->put(key, value);
}

template <typename KeyType, typename ValueType>
size_t HashLRUCache<KeyType, ValueType>::get_hash(KeyType key)
{
    std::hash<KeyType> hash_func;
    return hash_func(key);
}
