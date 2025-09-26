#pragma once

#include <memory>

template <typename KeyType, typename ValueType>
struct Node
{
    KeyType                                   key;
    ValueType                                 value;
    int                                       freq{1}; // LFU需要的频次字段，LRU可以忽略
    std::weak_ptr<Node<KeyType, ValueType>>   prev;    // 使用weak_ptr避免循环引用
    std::shared_ptr<Node<KeyType, ValueType>> next;

    Node() = default;
    Node(const KeyType& key, const ValueType& value) : key(key), value(value) {}
    Node(const KeyType& key, const ValueType& value, int freq) : key(key), value(value), freq(freq)
    {
    }
};
