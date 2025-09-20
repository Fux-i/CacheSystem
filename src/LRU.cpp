#include "BaseCache.cpp"
#include "log.cpp"
#include <cstddef>
#include <memory>
#include <unordered_map>

template <typename KeyType, typename ValueType>
struct Node
{
    KeyType               key;
    ValueType             value;
    std::weak_ptr<Node>   prev; // 使用weak_ptr避免循环引用
    std::shared_ptr<Node> next;

    Node() = default;
    Node(KeyType key, ValueType value) : key(key), value(value) {}
};

template <typename KeyType, typename ValueType>
class LRUCache : public BaseCache<KeyType, ValueType>
{
    using NodeType = Node<KeyType, ValueType>;
    using node_ptr = std::shared_ptr<Node<KeyType, ValueType>>;
    using node_map = std::unordered_map<KeyType, node_ptr>;

    size_t   capacity_;     // 最大容量
    size_t   node_count_{}; // 当前节点数量
    node_ptr first_;        // 虚拟头节点
    node_ptr last_;         // 虚拟尾节点
    node_map map_;          // 哈希表

  public:
    LRUCache(size_t capacity)
        : capacity_(capacity)
        , first_(std::make_shared<NodeType>())
        , last_(std::make_shared<NodeType>())
    {
        first_->next = last_;
        last_->prev  = first_;
    }

    bool get(KeyType key, ValueType& result) override
    {
        if (map_.find(key) != map_.end())
        {
            node_ptr temp = map_[key];
            move_to_first(temp);
            result = temp->value;
            log("[LRU get] get: ", key, " = ", result, '\n');
            return true;
        }
        log("[LRU get] get failed: ", key, '\n');
        return false;
    }

    ValueType get(KeyType key) override
    {
        ValueType result{};
        get(key, result);
        return result;
    }

    void put(KeyType key, ValueType value) override
    {
        if (map_.find(key) != map_.end())
        {
            node_ptr node = map_[key];
            node->value   = value;
            move_to_first(node);
            log("[LRU put] update: ", key, '=', value, '\n');
            return;
        }
        log("[LRU put] new put: ", key, '=', value, '\n');

        if (node_count_ < capacity_)
            node_count_++;
        else
            remove_last();

        auto node = std::make_shared<NodeType>(key, value);
        map_[key] = node;
        insert_first(node);
    }

    // 公开remove方法，供LRU-K等子类使用
    void remove_by_key(KeyType key)
    {
        auto it = map_.find(key);
        if (it != map_.end())
        {
            remove(it->second, true);
        }
    }

  private:
    void move_to_first(const node_ptr& node)
    {
        remove(node);
        insert_first(node);
    }

    void remove_last() { remove(last_->prev.lock(), true); }

    void remove(const node_ptr& node, const bool remove_map = false)
    {
        node->prev.lock()->next = node->next;
        node->next->prev        = node->prev.lock();
        node->next              = nullptr;

        if (remove_map)
            map_.erase(node->key);
    }

    void insert_first(const node_ptr& node)
    {
        node->prev         = first_;
        node->next         = first_->next;
        first_->next->prev = node;
        first_->next       = node;
    }
};
