#pragma once

#include "../utils/log.hpp"
#include "ARC.decl.hpp"

template <typename KeyType, typename ValueType>
ARCCache<KeyType, ValueType>::ARCCache(int capacity, int maxAverageFreq)
    : lruGhost_(std::make_shared<LRUCache<KeyType, ValueType>>(capacity))
    , lfuGhost_(std::make_shared<LRUCache<KeyType, ValueType>>(capacity))
    , lruPart_(std::make_unique<ArcLruPart<KeyType, ValueType>>(capacity, lruGhost_))
    , lfuPart_(
          std::make_unique<ArcLfuPart<KeyType, ValueType>>(capacity, maxAverageFreq, lfuGhost_))
{
    log("{ARC Constructor} ARC Cache initialized with capacity=",
        capacity,
        ", maxAverageFreq=",
        maxAverageFreq,
        "\n");
}

template <typename KeyType, typename ValueType>
bool ARCCache<KeyType, ValueType>::get(KeyType key, ValueType& result)
{
    log("{ARC get} Looking for key: ", key, "\n");

    // 首先在LRU部分查找
    if (lruPart_->get(key, result))
    {
        log("{ARC get} Found in LRU part: ", key, " = ", result, "\n");
        return true;
    }

    // 然后在LFU部分查找
    if (lfuPart_->get(key, result))
    {
        log("{ARC get} Found in LFU part: ", key, " = ", result, "\n");
        return true;
    }

    // 在LRU幽灵列表中查找，如果找到则移动到LFU部分
    if (lruGhost_->get(key, result))
    {
        log("{ARC get} Found in LRU ghost list: ", key, " -> promoting to LFU part\n");
        lfuPart_->put(key, result);
        lruGhost_->removeByKey(key);

        // 使用简化的容量管理策略
        lruPart_->changeCapacity(-1);
        lfuPart_->changeCapacity(1);
        log("{ARC get} Adjusted capacities: LRU-1, LFU+1\n");
        return true;
    }

    // 在LFU幽灵列表中查找，如果找到则移动到LRU部分
    if (lfuGhost_->get(key, result))
    {
        log("{ARC get} Found in LFU ghost list: ", key, " -> promoting to LRU part\n");
        lruPart_->put(key, result);
        lfuGhost_->removeByKey(key);

        lruPart_->changeCapacity(1);
        lfuPart_->changeCapacity(-1);
        log("{ARC get} Capacity adjustment disabled for stability\n");
        return true;
    }

    // 未找到
    log("{ARC get} Key not found: ", key, "\n");
    return false;
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
    log("{ARC put} Inserting key: ", key, ", value: ", value, "\n");
    ValueType temp{};

    // 如果key已在LRU部分，更新值
    if (lruPart_->get(key, temp))
    {
        log("{ARC put} Updating existing key in LRU part: ", key, " (", temp, " -> ", value, ")\n");
        lruPart_->put(key, value);
        return;
    }

    // 如果key已在LFU部分，更新值
    if (lfuPart_->get(key, temp))
    {
        log("{ARC put} Updating existing key in LFU part: ", key, " (", temp, " -> ", value, ")\n");
        lfuPart_->put(key, value);
        return;
    }

    // 如果key在LRU幽灵列表中，移动到LFU部分
    if (lruGhost_->get(key, temp))
    {
        log("{ARC put} Found in LRU ghost list: ", key, " -> promoting to LFU part\n");
        lfuPart_->put(key, value);
        lruGhost_->removeByKey(key);

        lruPart_->changeCapacity(-1);
        lfuPart_->changeCapacity(1);
        log("{ARC put} Capacity adjustment disabled for stability\n");
        return;
    }

    // 如果key在LFU幽灵列表中，移动到LRU部分
    if (lfuGhost_->get(key, temp))
    {
        log("{ARC put} Found in LFU ghost list: ", key, " -> promoting to LRU part\n");
        lruPart_->put(key, value);
        lfuGhost_->removeByKey(key);

        lruPart_->changeCapacity(1);
        lfuPart_->changeCapacity(-1);
        log("{ARC put} Capacity adjustment disabled for stability\n");
        return;
    }

    // 新key，默认插入到LRU部分
    log("{ARC put} New key: ", key, " -> inserting to LRU part\n");
    lruPart_->put(key, value);
}
