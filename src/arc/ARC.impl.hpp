#pragma once

#include "ARC.decl.hpp"

template <typename KeyType, typename ValueType>
ARCCache<KeyType, ValueType>::ARCCache(int capacity, int maxAverageFreq)
    : lruPart_(std::make_unique<LRUCache<KeyType, ValueType>>(capacity))
    , lfuPart_(std::make_unique<LFUCache<KeyType, ValueType>>(capacity, maxAverageFreq))
    , lruGhost_(std::make_unique<LRUCache<KeyType, ValueType>>(capacity))
    , lfuGhost_(std::make_unique<LRUCache<KeyType, ValueType>>(capacity))
{
}

template <typename KeyType, typename ValueType>
bool ARCCache<KeyType, ValueType>::get(KeyType key, ValueType& result)
{
    if (auto it = lruPart_.find(key); it != lruPart_.end())
    {
        lruPart_->get(key, result);
        return true;
    }
    if (auto it = lfuPart_.find(key); it != lfuPart_.end())
    {
        lfuPart_->get(key, result);
        return true;
    }
    if (auto it = lruGhost_.find(key); it != lruGhost_.end())
    {
        lruGhost_->get(key, result);
        lfuPart_->put(key, result);
        lruGhost_->removeByKey(key);
        return true;
    }
    if (auto it = lfuGhost_.find(key); it != lfuGhost_.end())
    {
        lfuGhost_->get(key, result);
        lruPart_->put(key, result);
        lfuGhost_->removeByKey(key);
        return true;
    }
}

template <typename KeyType, typename ValueType>
ValueType ARCCache<KeyType, ValueType>::get(KeyType key)
{
    ValueType result{};
    get(key, result);
    return result;
}

template <typename KeyType, typename ValueType>
void ARCCache<KeyType, ValueType>::put(KeyType key, ValueType value)
{
    if (auto it = lruPart_.find(key); it != lruPart_.end())
    {
        lruPart_->put(key, value);
        return;
    }
    if (auto it = lfuPart_.find(key); it != lfuPart_.end())
    {
        lfuPart_->put(key, value);
        return;
    }
    if (auto it = lruGhost_.find(key); it != lruGhost_.end())
    {
        lfuPart_->put(key, value);
        lruGhost_->removeByKey(key);
        return;
    }
    if (auto it = lfuGhost_.find(key); it != lfuGhost_.end())
    {
        lruGhost_->put(key, value);
        lfuGhost_->removeByKey(key);
        return;
    }
}
