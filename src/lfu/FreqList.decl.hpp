#pragma once

#include <memory>

template <typename KeyType, typename ValueType>
class LFUCache;

template <typename KeyType, typename ValueType>
class FreqList
{
    struct Node
    {
        int                   freq{};
        KeyType               key;
        ValueType             value;
        std::weak_ptr<Node>   prev; // 使用weak_ptr避免循环引用
        std::shared_ptr<Node> next;

        Node() = default;
        Node(KeyType key, ValueType value) : freq(1), key(key), value(value) {}
    };

    using NodePtr = std::shared_ptr<Node>;

    int     freq_;
    NodePtr head_;
    NodePtr tail_;

  public:
    FreqList(int freq);

    bool is_empty() const;

    void add_node(NodePtr node);

    void remove_node(NodePtr node);

    NodePtr get_earliest_node() const;

    friend class LFUCache<KeyType, ValueType>;
};
