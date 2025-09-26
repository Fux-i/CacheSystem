#pragma once

#include "../../lru/LRU/LRU.hpp"
#include <memory>

template <typename KeyType, typename ValueType>
class ArcLruPart : public LRUCache<KeyType, ValueType>
{
    std::shared_ptr<LRUCache<KeyType, ValueType>> ghostList_;

  public:
    ArcLruPart(int capacity, std::shared_ptr<LRUCache<KeyType, ValueType>> ghostList_);

  protected:
    void removeLast() override;
};