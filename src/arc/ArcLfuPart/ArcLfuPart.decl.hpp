#pragma once

#include "../../lfu/LFU/LFU.hpp"
#include "../../lru/LRU/LRU.hpp"
#include <memory>

template <typename KeyType, typename ValueType>
class ArcLfuPart : public LFUCache<KeyType, ValueType>
{
    std::shared_ptr<LRUCache<KeyType, ValueType>> ghostList_;

  public:
    ArcLfuPart(int capacity, int maxAverageFreq,
               std::shared_ptr<LRUCache<KeyType, ValueType>> ghostList_);

  protected:
    void removeLast() override;
};