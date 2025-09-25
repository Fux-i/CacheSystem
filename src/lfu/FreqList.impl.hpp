#include "FreqList.decl.hpp"
#include <string>

template <typename KeyType, typename ValueType>
FreqList<KeyType, ValueType>::FreqList(int freq)
    : freq_(freq), head_(std::make_shared<Node>()), tail_(std::make_shared<Node>())
{
    head_->next = tail_;
    tail_->prev = head_;
}

template <typename KeyType, typename ValueType>
bool FreqList<KeyType, ValueType>::empty() const
{
    return head_->next == tail_;
}

template <typename KeyType, typename ValueType>
void FreqList<KeyType, ValueType>::addNode(typename FreqList<KeyType, ValueType>::NodePtr node)
{
    node->next        = head_->next;
    node->prev        = head_;
    head_->next->prev = node;
    head_->next       = node;
}

template <typename KeyType, typename ValueType>
void FreqList<KeyType, ValueType>::removeNode(typename FreqList<KeyType, ValueType>::NodePtr node)
{
    node->prev.lock()->next = node->next;
    node->next->prev        = node->prev;

    node->next.reset();
    node->prev.reset();
}

template <typename KeyType, typename ValueType>
typename FreqList<KeyType, ValueType>::NodePtr FreqList<KeyType, ValueType>::getEarliestNode() const
{
    return tail_->prev.lock();
}

// 显式模板实例化
template class FreqList<std::string, int>;