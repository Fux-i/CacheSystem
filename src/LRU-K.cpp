#include "LRU.cpp"
#include <memory>

template <typename KeyType, typename ValueType>
class LRUKCache : public LRUCache<KeyType, ValueType>
{
    using mapType = std::unordered_map<KeyType, ValueType>;

    int     k_;           // 访问多少次进入缓存
    mapType history_map_; // 存储访问次数未达到k次的数据
    std::unique_ptr<LRUCache<KeyType, size_t>> history_cache_; // 访问数据历史记录

  public:
    LRUKCache(int k, size_t capacity, size_t history_capacity)
        : k_(k)
        , LRUCache<KeyType, ValueType>(capacity)
        , history_cache_(std::make_unique<LRUCache<KeyType, size_t>>(history_capacity))
    {
    }

    bool get(KeyType key, ValueType& result) override
    {
        // 尝试从主缓存中获取数据
        bool inMainCache = LRUCache<KeyType, ValueType>::get(key, result);
        if (inMainCache)
        {
            log("[LRU-K get] get cached: ", key, " = ", result, '\n');
            return true;
        }

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
                result = it->second;
                // 删除记录
                history_map_.erase(it);
                history_cache_->remove_by_key(key);

                // 添加到主缓存
                LRUCache<KeyType, ValueType>::put(key, result);
                log("[LRU-K get] add to main cache: ", key, '=', result, '\n');

                return true;
            }
        }
        return false;
    }

    void put(KeyType key, ValueType value) override
    {
        ValueType existing_value;
        bool      inMainCache = LRUCache<KeyType, ValueType>::get(key, existing_value);
        // 已在主缓存中，直接更新
        if (inMainCache)
        {
            LRUCache<KeyType, ValueType>::put(key, value);
            log("[LRU-K put get] update main cache: ", key, '=', value, '\n');
            return;
        }

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
            // 达到 k 次，将数据放入主缓存
            LRUCache<KeyType, ValueType>::put(key, value);
            log("[LRU-K put] add to main cache: ", key, '=', value, '\n');
            // 从历史记录中移除
            history_cache_->remove_by_key(key);
            history_map_.erase(key);
        }
    }
};