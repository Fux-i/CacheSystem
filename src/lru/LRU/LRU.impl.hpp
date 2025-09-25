#pragma once

#include "../../utils/log.hpp"
#include "LRU.decl.hpp"

template <typename KeyType, typename ValueType>
LRUCache<KeyType, ValueType>::LRUCache(int capacity)
    : capacity_(capacity), first_(std::make_shared<NodeType>()), last_(std::make_shared<NodeType>())
{
    first_->next = last_;
    last_->prev  = first_;
}

template <typename KeyType, typename ValueType>
bool LRUCache<KeyType, ValueType>::get(KeyType key, ValueType& result)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    if (map_.find(key) != map_.end())
    {
        NodePtr temp = map_[key];
        moveToFirst(temp);
        result = temp->value;
        log("(LRU get) get: ", key, " = ", result, '\n');
        return true;
    }
    log("(LRU get) get failed: ", key, '\n');
    return false;
}

template <typename KeyType, typename ValueType>
ValueType LRUCache<KeyType, ValueType>::get(KeyType key)
{
    ValueType result{};
    get(key, result);
    return result;
}

template <typename KeyType, typename ValueType>
void LRUCache<KeyType, ValueType>::put(KeyType key, ValueType value)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    if (map_.find(key) != map_.end())
    {
        NodePtr node = map_[key];
        node->value  = value;
        moveToFirst(node);
        log("(LRU put) update: ", key, '=', value, '\n');
        return;
    }
    log("(LRU put) new put: ", key, '=', value, '\n');

    if (nodeCount_ < capacity_)
        nodeCount_++;
    else
        removeLast();

    auto node = std::make_shared<NodeType>(key, value);
    map_[key] = node;
    insertFirst(node);
}

template <typename KeyType, typename ValueType>
void LRUCache<KeyType, ValueType>::removeByKey(KeyType key)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    auto                                it = map_.find(key);
    if (it != map_.end())
    {
        remove(it->second, true);
        nodeCount_--;
    }
}

template <typename KeyType, typename ValueType>
void LRUCache<KeyType, ValueType>::moveToFirst(const NodePtr& node)
{
    remove(node);
    insertFirst(node);
}

template <typename KeyType, typename ValueType>
void LRUCache<KeyType, ValueType>::removeLast()
{
    remove(last_->prev.lock(), true);
}

template <typename KeyType, typename ValueType>
void LRUCache<KeyType, ValueType>::remove(const NodePtr& node, const bool removeMap)
{
    node->prev.lock()->next = node->next;
    node->next->prev        = node->prev.lock();
    node->next              = nullptr;

    if (removeMap)
        map_.erase(node->key);
}

template <typename KeyType, typename ValueType>
void LRUCache<KeyType, ValueType>::insertFirst(const NodePtr& node)
{
    node->prev         = first_;
    node->next         = first_->next;
    first_->next->prev = node;
    first_->next       = node;
}
