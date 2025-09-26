#pragma once

#include "../common/Node.hpp"
#include <memory>

template <typename KeyType, typename ValueType>
class LFUCache;

template <typename KeyType, typename ValueType>
class FreqList
{
    using NodeType = Node<KeyType, ValueType>;
    using NodePtr  = std::shared_ptr<NodeType>;

    int     freq_;
    NodePtr head_;
    NodePtr tail_;

  public:
    FreqList(int freq);

    bool empty() const;

    void addNode(NodePtr node);

    void removeNode(NodePtr node);

    NodePtr getEarliestNode() const;

    friend class LFUCache<KeyType, ValueType>;
};
