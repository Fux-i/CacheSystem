#pragma once

#include "../../common/BaseCache.hpp"
#include "../../common/Node.hpp"
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

template <typename KeyType, typename ValueType>
class LRUCache : public BaseCache<KeyType, ValueType>
{
    using NodeType = Node<KeyType, ValueType>;
    using NodePtr  = std::shared_ptr<NodeType>;
    using NodeMap  = std::unordered_map<KeyType, NodePtr>;

    int     capacity_;    // 最大容量
    int     nodeCount_{}; // 当前节点数量
    NodePtr first_;       // 虚拟头节点
    NodePtr last_;        // 虚拟尾节点
    NodeMap map_;         // 哈希表

    mutable std::shared_mutex mutex_; // 读写锁，支持多个读线程并发访问

  public:
    LRUCache(int capacity);

    bool      get(KeyType key, ValueType& result) override;
    ValueType get(KeyType key) override;
    void      put(KeyType key, ValueType value) override;

    // 公开remove方法，供LRU-K等子类使用
    void removeByKey(KeyType key);

  protected:
    void removeLast();
    void remove(const NodePtr& node, bool removeMap = false);

  private:
    void moveToFirst(const NodePtr& node);
    void insertFirst(const NodePtr& node);
};
