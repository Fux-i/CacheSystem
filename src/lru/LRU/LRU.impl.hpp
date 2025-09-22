#pragma once

#include "../../utils/log.cpp"
#include "LRU.decl.hpp"

template <typename KeyType, typename ValueType>
Node<KeyType, ValueType>::Node(KeyType key, ValueType value) : key(key), value(value)
{
}

template <typename KeyType, typename ValueType>
LRUCache<KeyType, ValueType>::LRUCache(size_t capacity)
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
        node_ptr temp = map_[key];
        move_to_first(temp);
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
        node_ptr node = map_[key];
        node->value   = value;
        move_to_first(node);
        log("(LRU put) update: ", key, '=', value, '\n');
        return;
    }
    log("(LRU put) new put: ", key, '=', value, '\n');

    if (node_count_ < capacity_)
        node_count_++;
    else
        remove_last();

    auto node = std::make_shared<NodeType>(key, value);
    map_[key] = node;
    insert_first(node);
}

template <typename KeyType, typename ValueType>
void LRUCache<KeyType, ValueType>::remove_by_key(KeyType key)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    auto                                it = map_.find(key);
    if (it != map_.end())
    {
        remove(it->second, true);
        node_count_--;
    }
}

template <typename KeyType, typename ValueType>
void LRUCache<KeyType, ValueType>::move_to_first(const node_ptr& node)
{
    remove(node);
    insert_first(node);
}

template <typename KeyType, typename ValueType>
void LRUCache<KeyType, ValueType>::remove_last()
{
    remove(last_->prev.lock(), true);
}

template <typename KeyType, typename ValueType>
void LRUCache<KeyType, ValueType>::remove(const node_ptr& node, const bool remove_map)
{
    node->prev.lock()->next = node->next;
    node->next->prev        = node->prev.lock();
    node->next              = nullptr;

    if (remove_map)
        map_.erase(node->key);
}

template <typename KeyType, typename ValueType>
void LRUCache<KeyType, ValueType>::insert_first(const node_ptr& node)
{
    node->prev         = first_;
    node->next         = first_->next;
    first_->next->prev = node;
    first_->next       = node;
}
