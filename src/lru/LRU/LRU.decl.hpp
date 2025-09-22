#pragma once

#include "../../BaseCache.hpp"
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

template <typename KeyType, typename ValueType>
struct Node
{
    KeyType               key;
    ValueType             value;
    std::weak_ptr<Node>   prev; // 使用weak_ptr避免循环引用
    std::shared_ptr<Node> next;

    Node() = default;
    Node(KeyType key, ValueType value);
};

template <typename KeyType, typename ValueType>
class LRUCache : public BaseCache<KeyType, ValueType>
{
    using NodeType = Node<KeyType, ValueType>;
    using node_ptr = std::shared_ptr<Node<KeyType, ValueType>>;
    using node_map = std::unordered_map<KeyType, node_ptr>;

    size_t   capacity_;     // 最大容量
    size_t   node_count_{}; // 当前节点数量
    node_ptr first_;        // 虚拟头节点
    node_ptr last_;         // 虚拟尾节点
    node_map map_;          // 哈希表

    mutable std::shared_mutex mutex_; // 读写锁，支持多个读线程并发访问

  public:
    LRUCache(size_t capacity);

    bool      get(KeyType key, ValueType& result) override;
    ValueType get(KeyType key) override;
    void      put(KeyType key, ValueType value) override;

    // 公开remove方法，供LRU-K等子类使用
    void remove_by_key(KeyType key);

  private:
    void move_to_first(const node_ptr& node);
    void remove_last();
    void remove(const node_ptr& node, bool remove_map = false);
    void insert_first(const node_ptr& node);
};
