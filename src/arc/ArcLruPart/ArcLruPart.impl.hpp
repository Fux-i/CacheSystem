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
    auto lastNode = this->getLastNode();
    if (lastNode)
    {
        // 检查是否是有效的数据节点（不是哨兵节点）
        try
        {
            // 尝试访问key，如果是哨兵节点或无效节点，这里可能会有问题
            auto key   = lastNode->key;
            auto value = lastNode->value;

            // 检查是否是空key（可能是哨兵节点）
            if (key.empty())
            {
                log("{ARC-LRU} Warning: found node with empty key, skipping eviction\n");
                return;
            }

            log("{ARC-LRU} Evicting last node: ", key, " -> moving to LRU ghost list\n");
            ghostList_->put(key, value);
            this->remove(lastNode, true);
        }
        catch (...)
        {
            log("{ARC-LRU} Warning: Invalid node detected, skipping eviction\n");
        }
    }
    else
    {
        log("{ARC-LRU} Warning: getLastNode returned nullptr\n");
    }
}