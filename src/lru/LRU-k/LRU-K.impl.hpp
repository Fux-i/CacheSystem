#pragma once

#include "../../utils/log.hpp"
#include "LRU-K.decl.hpp"

template <typename KeyType, typename ValueType>
LRUKCache<KeyType, ValueType>::LRUKCache(size_t k, size_t capacity, size_t history_capacity)
    : LRUCache<KeyType, ValueType>(capacity)
    , k_(k)
    , history_cache_(std::make_unique<LRUCache<KeyType, size_t>>(history_capacity))
{
}

template <typename KeyType, typename ValueType>
bool LRUKCache<KeyType, ValueType>::get(KeyType key, ValueType& result)
{
    ValueType temp_value;
    bool      should_promote = false;
    bool      inMainCache    = false;

    // 使用锁保护整个检查和历史记录操作
    {
        std::lock_guard<std::mutex> history_lock(history_mutex_);

        // 尝试从主缓存中获取数据
        inMainCache = LRUCache<KeyType, ValueType>::get(key, result);
        if (!inMainCache)
        {
            // 获取并更新访问历史计数
            size_t historyCount = history_cache_->get(key); // 若无则返回默认值
            historyCount++;
            log("[LRU-K get] update access count: ", key, " count=", historyCount, '\n');
            history_cache_->put(key, historyCount);

            // 若不在主缓存，且访问次数达到 k 次
            if (historyCount >= k_)
            {
                log("[LRU-K get] count reached k\n");
                auto it = history_map_.find(key);
                if (it != history_map_.end())
                {
                    temp_value = it->second;
                    // 删除历史记录
                    history_map_.erase(it);
                    history_cache_->remove_by_key(key);
                    should_promote = true;
                }
            }
        }
    } // history_lock自动释放

    // 处理结果
    if (inMainCache)
    {
        log("[LRU-K get] get cached: ", key, " = ", result, '\n');
        return true;
    }

    if (should_promote)
    {
        // 在锁外添加到主缓存
        LRUCache<KeyType, ValueType>::put(key, temp_value);
        log("[LRU-K get] add to main cache: ", key, '=', temp_value, '\n');
        result = temp_value;
        return true;
    }

    return false;
}

template <typename KeyType, typename ValueType>
ValueType LRUKCache<KeyType, ValueType>::get(KeyType key)
{
    ValueType result{};
    get(key, result);
    return result;
}

template <typename KeyType, typename ValueType>
void LRUKCache<KeyType, ValueType>::put(KeyType key, ValueType value)
{
    bool should_promote = false;
    bool inMainCache    = false;

    // 使用锁保护检查和历史记录操作，避免TOCTOU竞态条件
    {
        std::lock_guard<std::mutex> history_lock(history_mutex_);

        ValueType existing_value;
        inMainCache = LRUCache<KeyType, ValueType>::get(key, existing_value);
        // 已在主缓存中，标记需要更新
        if (!inMainCache)
        {
            // 不在主缓存，更新访问历史
            size_t history_count = history_cache_->get(key);
            history_count++;
            log("[LRU-K put] update access count: ", key, " count=", history_count, '\n');
            history_cache_->put(key, history_count);

            // 保存值到历史记录映射，供后续 get 操作使用
            history_map_[key] = value;

            // 检查是否达到 k 次
            if (history_count >= k_)
            {
                // 从历史记录中移除
                history_cache_->remove_by_key(key);
                history_map_.erase(key);
                should_promote = true;
            }
        }
    } // history_lock自动释放

    // 在锁外进行主缓存操作
    if (inMainCache)
    {
        LRUCache<KeyType, ValueType>::put(key, value);
        log("[LRU-K put get] update main cache: ", key, '=', value, '\n');
    }
    else if (should_promote)
    {
        // 达到 k 次，将数据放入主缓存
        LRUCache<KeyType, ValueType>::put(key, value);
        log("[LRU-K put] add to main cache: ", key, '=', value, '\n');
    }
}
