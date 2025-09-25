#pragma once

#include <memory>

namespace cache_system
{

    template <typename KeyType, typename ValueType>
    struct Node
    {
        KeyType               key;
        ValueType             value;
        int                   freq{1}; // LFU需要的频次字段，LRU可以忽略
        std::weak_ptr<Node>   prev;    // 使用weak_ptr避免循环引用
        std::shared_ptr<Node> next;

        Node() = default;
        Node(KeyType key, ValueType value) : key(key), value(value) {}
        Node(KeyType key, ValueType value, int freq) : key(key), value(value), freq(freq) {}
    };

} // namespace cache_system
