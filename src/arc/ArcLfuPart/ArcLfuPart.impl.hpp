#pragma once

#include "../../utils/log.hpp"
#include "ArcLfuPart.decl.hpp"
#include <memory>

template <typename KeyType, typename ValueType>
ArcLfuPart<KeyType, ValueType>::ArcLfuPart(int capacity, int maxAverageFreq,
                                           std::shared_ptr<LRUCache<KeyType, ValueType>> ghostList_)
    : LFUCache<KeyType, ValueType>(capacity, maxAverageFreq), ghostList_(ghostList_)
{
}

template <typename KeyType, typename ValueType>
void ArcLfuPart<KeyType, ValueType>::removeLast()
{
    auto node = this->getLastNode();
    if (!node)
    {
        log("{ARC-LFU} Warning: getLastNode returned nullptr\n");
        return;
    }

    try
    {
        auto key   = node->key;
        auto value = node->value;
        auto freq  = node->freq;

        log("{ARC-LFU} Evicting last node: ",
            key,
            " (freq: ",
            freq,
            ") -> moving to LFU ghost list\n");
        ghostList_->put(key, value);
        this->remove(node, true);
        this->decreaseTotalFreq(freq);
    }
    catch (...)
    {
        log("{ARC-LFU} Warning: Invalid node detected, skipping eviction\n");
    }
}