#pragma once

#include "ArcLruPart.decl.hpp"
#include <memory>

template <typename KeyType, typename ValueType>
ArcLruPart<KeyType, ValueType>::ArcLruPart(int                                           capacity,
                                           std::unique_ptr<LRUCache<KeyType, ValueType>> ghostList_)
    : LRUCache<KeyType, ValueType>(capacity), ghostList_(std::move(ghostList_))
{
}

template <typename KeyType, typename ValueType>
void ArcLruPart<KeyType, ValueType>::removeLast()
{
    auto lastNode = this->last_->prev.lock();
    if (lastNode && lastNode != this->first_)
    {
        ghostList_->put(lastNode->key, lastNode->value);
        this->remove(lastNode, true);
    }
}