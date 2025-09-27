#pragma once

#include "../../utils/log.hpp"
#include "ArcLruPart.decl.hpp"
#include <memory>

template <typename KeyType, typename ValueType>
ArcLruPart<KeyType, ValueType>::ArcLruPart(int                                           capacity,
                                           std::shared_ptr<LRUCache<KeyType, ValueType>> ghostList_)
    : LRUCache<KeyType, ValueType>(capacity), ghostList_(ghostList_)
{
}

template <typename KeyType, typename ValueType>
void ArcLruPart<KeyType, ValueType>::removeLast()
{
    // 使用基类提供的安全接口检查
    if (!this->hasValidNodes())
    {
        log("{ARC-LRU} Warning: No valid nodes to evict\n");
        return;
    }

    auto lastNode = this->getLastNode();

    auto key   = lastNode->key;
    auto value = lastNode->value;

    log("{ARC-LRU} Evicting last node: ", key, " -> moving to LRU ghost list\n");
    ghostList_->put(key, value);
    this->remove(lastNode, true);
}