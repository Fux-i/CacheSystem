#pragma once

#include "../../common/BaseCache.hpp"
#include "../FreqList.decl.hpp"
#include <memory>
#include <shared_mutex>
#include <unordered_map>

template <typename KeyType, typename ValueType>
class LFUCache : public BaseCache<KeyType, ValueType>
{
    using NodeType = typename FreqList<KeyType, ValueType>::Node;
    using NodePtr  = std::shared_ptr<NodeType>;
    using NodeMap  = std::unordered_map<KeyType, NodePtr>;
    using FreqMap  = std::unordered_map<int, std::unique_ptr<FreqList<KeyType, ValueType>>>;

    int capacity_;         // 缓存容量
    int min_freq_;         // 最小频次（用于找到最小访问频次节点）
    int max_average_freq_; // 最大平均频次
    int cur_average_freq_; // 当前平均频次
    int cur_total_freq_;   // 总频次

    NodeMap node_map_; // key->node
    FreqMap freq_map_; // freq->freq_list

    mutable std::mutex mutex_; // 互斥锁，保护node_map_和freq_map_

  public:
    LFUCache(int capacity, int max_average_freq);

    bool      get(KeyType key, ValueType& result) override;
    ValueType get(KeyType key) override;
    void      put(KeyType key, ValueType value) override;

    /**
     * @brief 清空数据
     */
    void purge();

  protected:
    /**
     * @brief 移除缓存中最不常访问的数据
     */
    void kickOut();

    /**
     * @brief 从频率列表中移除节点
     * @param node 节点
     */
    void removeFromFreqList(NodePtr node);

    /**
     * @brief 减少平均访问等频率
     * @param num 减少的频率
     */
    void decreaseTotalFreq(int num);

  private:
    /**
     * @brief 添加缓存
     * @param key 键
     * @param value 值
     */
    void putInternal(KeyType key, ValueType value);

    /**
     * @brief 获取缓存
     * @param node 节点
     * @param value 值
     */
    void getInternal(NodePtr node, ValueType& value);

    /**
     * @brief 添加到频率列表
     * @param node 节点
     */
    void addToFreqList(NodePtr node);

    /**
     * @brief 增加平均访问等频率
     */
    void addTotalFreq();

    /**
     * @brief 处理当前平均访问频率超过上限的情况
     */
    void handleOverMaxAverageNum();

    /**
     * @brief 更新最小频次
     */
    void updateMinFreq();
};
