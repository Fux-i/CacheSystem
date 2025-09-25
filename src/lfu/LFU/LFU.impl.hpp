#pragma once

#include "../../utils/log.hpp"
#include "LFU.decl.hpp"
#include <cstdint>

template <typename KeyType, typename ValueType>
LFUCache<KeyType, ValueType>::LFUCache(int capacity, int max_average_freq)
    : capacity_(capacity)
    , min_freq_(INT32_MAX)
    , max_average_freq_(max_average_freq)
    , cur_average_freq_(0)
    , cur_total_freq_(0)
{
    log("[LFU Constructor] LFUCache initialized with capacity=",
        capacity_,
        ", max_average_freq=",
        max_average_freq_,
        ", min_freq_=",
        min_freq_,
        '\n');
}

template <typename KeyType, typename ValueType>
bool LFUCache<KeyType, ValueType>::get(KeyType key, ValueType& result)
{
    std::lock_guard<std::mutex> lock(mutex_);

    log("[LFU get] Looking for key: ", key, '\n');

    auto it = node_map_.find(key);
    if (it == node_map_.end())
    {
        log("[LFU get] Key not found: ", key, '\n');
        return false;
    }

    NodePtr node = it->second;
    if (!node)
    {
        log("[LFU get] Node is null for key: ", key, '\n');
        return false;
    }

    log("[LFU get] Found key: ",
        key,
        ", current freq: ",
        node->freq,
        ", value: ",
        node->value,
        '\n');

    getInternal(node, result);

    log("[LFU get] Successfully retrieved key: ",
        key,
        ", updated freq: ",
        node->freq,
        ", returned value: ",
        result,
        '\n');

    return true;
}

template <typename KeyType, typename ValueType>
ValueType LFUCache<KeyType, ValueType>::get(KeyType key)
{
    ValueType result{};
    get(key, result);
    return result;
}

template <typename KeyType, typename ValueType>
void LFUCache<KeyType, ValueType>::put(KeyType key, ValueType value)
{
    std::lock_guard<std::mutex> lock(mutex_);

    log("[LFU put] Inserting key: ", key, ", value: ", value, '\n');

    // 检查是否已存在
    auto it = node_map_.find(key);
    if (it != node_map_.end())
    {
        NodePtr node = it->second;
        if (node)
        {
            int old_value = -1;
            getInternal(node, old_value);
            log("[LFU put] Key already exists: ",
                key,
                ", ",
                old_value,
                "->",
                value,
                ", freq: ",
                node->freq,
                '\n');

            node->value = value;
            return;
        }

        log("[LFU put] Node is null for key: ", key, '\n');
    }

    log("[LFU put] Key is new, current size: ", node_map_.size(), "/", capacity_, '\n');

    putInternal(key, value);

    log("[LFU put] Successfully inserted key: ",
        key,
        ", final size: ",
        node_map_.size(),
        "/",
        capacity_,
        '\n');
}

template <typename KeyType, typename ValueType>
void LFUCache<KeyType, ValueType>::purge()
{
    std::lock_guard<std::mutex> lock(mutex_);
    node_map_.clear();
    freq_map_.clear();
}

template <typename KeyType, typename ValueType>
void LFUCache<KeyType, ValueType>::putInternal(KeyType key, ValueType value)
{
    log("[LFU putInternal] Adding new key: ", key, ", value: ", value, '\n');

    if (node_map_.size() >= capacity_)
    {
        log("[LFU putInternal] Cache is full (",
            node_map_.size(),
            "/",
            capacity_,
            "), need to evict\n");
        kickOut();
    }

    log("[LFU putInternal] Creating new node for key: ", key, '\n');
    NodePtr node   = std::make_shared<NodeType>(key, value);
    node_map_[key] = node;
    addTotalFreq();
    addToFreqList(node);
    min_freq_ = std::min(min_freq_, 1);

    log("[LFU putInternal] Successfully added key: ",
        key,
        ", current size: ",
        node_map_.size(),
        "/",
        capacity_,
        ", min_freq: ",
        min_freq_,
        '\n');
}

template <typename KeyType, typename ValueType>
void LFUCache<KeyType, ValueType>::getInternal(NodePtr node, ValueType& value)
{
    value        = node->value;
    int old_freq = node->freq;

    log("[LFU getInternal] Processing access for key: ",
        node->key,
        ", old freq: ",
        old_freq,
        ", value: ",
        value,
        '\n');

    removeFromFreqList(node);
    node->freq++;
    addToFreqList(node);

    // 更新总频次（访问时频次增加1）
    cur_total_freq_++;
    int old_average = cur_average_freq_;
    if (node_map_.empty())
        cur_average_freq_ = 0;
    else
        cur_average_freq_ = cur_total_freq_ / static_cast<int>(node_map_.size());

    log("[LFU getInternal] Updated frequency for key: ",
        node->key,
        " from ",
        old_freq,
        " to ",
        node->freq,
        ", total freq: ",
        cur_total_freq_,
        ", average freq: ",
        old_average,
        " -> ",
        cur_average_freq_,
        '\n');

    // 检查是否超过最大平均频次
    if (cur_average_freq_ > max_average_freq_)
    {
        log("[LFU getInternal] Average freq (",
            cur_average_freq_,
            ") exceeds max (",
            max_average_freq_,
            "), handling overflow\n");
        handleOverMaxAverageNum();
    }

    // 若现在 freq = min_freq_+1 且无 min_freq_的 node 了，更新 min_freq_
    if (node->freq == min_freq_ + 1 && freq_map_[min_freq_]->empty())
    {
        int old_min_freq = min_freq_;
        min_freq_++;
        log("[LFU getInternal] Updated min_freq from ",
            old_min_freq,
            " to ",
            min_freq_,
            " (no nodes at old min_freq)\n");
    }
}

template <typename KeyType, typename ValueType>
void LFUCache<KeyType, ValueType>::kickOut()
{
    log("[LFU kickOut] Evicting least frequently used node, min_freq: ", min_freq_, '\n');

    auto node = freq_map_[min_freq_]->getEarliestNode();
    if (!node)
    {
        log("[LFU kickOut] No node found to evict!\n");
        return;
    }

    log("[LFU kickOut] Evicting key: ",
        node->key,
        ", freq: ",
        node->freq,
        ", value: ",
        node->value,
        '\n');

    node_map_.erase(node->key);
    removeFromFreqList(node);

    decreaseTotalFreq(node->freq);

    log("[LFU kickOut] Successfully evicted key: ",
        node->key,
        ", new size: ",
        node_map_.size(),
        "/",
        capacity_,
        '\n');
}

template <typename KeyType, typename ValueType>
void LFUCache<KeyType, ValueType>::removeFromFreqList(NodePtr node)
{
    if (!node)
        return;

    int freq = node->freq;
    log("[LFU removeFromFreqList] Removing node with key: ",
        node->key,
        " from freq list: ",
        freq,
        '\n');

    // 检查频率列表是否存在
    if (freq_map_.find(freq) != freq_map_.end())
    {
        freq_map_[freq]->removeNode(node);
    }

    log("[LFU removeFromFreqList] Successfully removed node with key: ",
        node->key,
        " from freq list: ",
        freq,
        '\n');
}

template <typename KeyType, typename ValueType>
void LFUCache<KeyType, ValueType>::addToFreqList(NodePtr node)
{
    if (!node)
        return;

    int freq = node->freq;
    log("[LFU addToFreqList] Adding node with key: ", node->key, " to freq list: ", freq, '\n');

    // 如果频率列表不存在，创建一个新的
    if (freq_map_.find(freq) == freq_map_.end())
    {
        freq_map_[freq] = std::make_unique<FreqList<KeyType, ValueType>>(freq);
    }

    freq_map_[freq]->addNode(node);

    log("[LFU addToFreqList] Successfully added node with key: ",
        node->key,
        " to freq list: ",
        freq,
        '\n');
}

template <typename KeyType, typename ValueType>
void LFUCache<KeyType, ValueType>::addTotalFreq()
{
    // 新添加节点，频次为1
    cur_total_freq_ += 1;
    int old_average = cur_average_freq_;
    if (node_map_.empty())
        cur_average_freq_ = 0;
    else
        cur_average_freq_ = cur_total_freq_ / static_cast<int>(node_map_.size());

    log("[LFU addTotalFreq] Updated total freq: ",
        cur_total_freq_,
        ", average freq: ",
        old_average,
        " -> ",
        cur_average_freq_,
        '\n');

    if (cur_average_freq_ > max_average_freq_)
    {
        log("[LFU addTotalFreq] Average freq (",
            cur_average_freq_,
            ") exceeds max (",
            max_average_freq_,
            "), handling overflow\n");
        handleOverMaxAverageNum();
    }
}

template <typename KeyType, typename ValueType>
void LFUCache<KeyType, ValueType>::decreaseTotalFreq(int num)
{
    int old_total = cur_total_freq_;
    cur_total_freq_ -= num;

    int old_average = cur_average_freq_;
    if (node_map_.empty())
        cur_average_freq_ = 0;
    else
        cur_average_freq_ = cur_total_freq_ / node_map_.size();

    log("[LFU decreaseTotalFreq] Decreased total freq from ",
        old_total,
        " to ",
        cur_total_freq_,
        " (subtracted ",
        num,
        "), average freq: ",
        old_average,
        " -> ",
        cur_average_freq_,
        '\n');
}

template <typename KeyType, typename ValueType>
void LFUCache<KeyType, ValueType>::handleOverMaxAverageNum()
{
    int reduction = max_average_freq_ / 2;
    log("[LFU handleOverMaxAverageNum] Handling frequency overflow, reducing all frequencies by ",
        reduction,
        '\n');

    // 重新计算总频次
    cur_total_freq_ = 0;

    for (auto it = node_map_.begin(); it != node_map_.end(); ++it)
    {
        // node 不存在，跳过
        if (!it->second)
            continue;

        NodePtr node     = it->second;
        int     old_freq = node->freq;

        log("[LFU handleOverMaxAverageNum] Processing key: ",
            node->key,
            ", old freq: ",
            old_freq,
            '\n');

        removeFromFreqList(node);

        node->freq -= reduction;
        node->freq = std::max(node->freq, 1);

        addToFreqList(node);

        // 累计新的总频次
        cur_total_freq_ += node->freq;

        log("[LFU handleOverMaxAverageNum] Updated key: ",
            node->key,
            " freq: ",
            old_freq,
            " -> ",
            node->freq,
            '\n');
    }

    // 更新平均频次
    if (node_map_.empty())
        cur_average_freq_ = 0;
    else
        cur_average_freq_ = cur_total_freq_ / static_cast<int>(node_map_.size());

    log("[LFU handleOverMaxAverageNum] Updated total freq: ",
        cur_total_freq_,
        ", average freq: ",
        cur_average_freq_,
        '\n');

    updateMinFreq();
}

template <typename KeyType, typename ValueType>
void LFUCache<KeyType, ValueType>::updateMinFreq()
{
    int old_min_freq = min_freq_;
    min_freq_        = INT32_MAX;

    log("[LFU updateMinFreq] Updating min_freq, current: ", old_min_freq, '\n');

    for (auto it = freq_map_.begin(); it != freq_map_.end(); ++it)
    {
        if (!it->second->empty())
        {
            min_freq_ = std::min(min_freq_, it->first);
            log("[LFU updateMinFreq] Found non-empty freq list: ", it->first, '\n');
        }
    }

    if (min_freq_ == INT32_MAX)
    {
        min_freq_ = 1;
        log("[LFU updateMinFreq] No non-empty freq lists found, setting min_freq to 1\n");
    }
    else
        log("[LFU updateMinFreq] Updated min_freq from ", old_min_freq, " to ", min_freq_, '\n');
}