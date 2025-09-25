#pragma once

#include "ArcLfuPart.decl.hpp"
#include <memory>

template <typename KeyType, typename ValueType>
ArcLfuPart<KeyType, ValueType>::ArcLfuPart(int                                           capacity,
                                           std::unique_ptr<LFUCache<KeyType, ValueType>> ghostList_)
    : LFUCache<KeyType, ValueType>(capacity), ghostList_(std::move(ghostList_))
{
}

template <typename KeyType, typename ValueType>
void ArcLfuPart<KeyType, ValueType>::kickOut()
{
    auto node = this->freq_map_[this->min_freq_]->getEarliestNode();
    if (!node)
        return;
    
    ghostList_->put(node->key, node->value);

    this->node_map_.erase(node->key);
    this->removeFromFreqList(node);

    this->decreaseTotalFreq(node->freq);
}