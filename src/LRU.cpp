#include <memory>
#include <unordered_map>

struct Node
{
    int                   key;
    int                   value;
    std::weak_ptr<Node>   prev;
    std::shared_ptr<Node> next;

    Node(int key, int value) : key(key), value(value) {}
};

class LRUCache
{
    using node_ptr = std::shared_ptr<Node>;

    int                               capacity_;
    int                               node_count_{};
    node_ptr                          first_;
    node_ptr                          last_;
    std::unordered_map<int, node_ptr> map_;

  public:
    LRUCache(int capacity) : capacity_(capacity)
    {
        first_       = std::make_shared<Node>(-1, -1);
        last_        = std::make_shared<Node>(-1, -1);
        first_->next = last_;
        last_->prev  = first_;
    }

    int get(int key)
    {
        if (map_.find(key) != map_.end())
        {
            node_ptr temp = map_[key];
            move_to_first(temp);
            return temp->value;
        }
        return -1;
    }

    void put(int key, int value)
    {
        if (map_.find(key) != map_.end())
        {
            node_ptr node = map_[key];
            node->value   = value;
            move_to_first(node);
            return;
        }

        if (node_count_ < capacity_)
            node_count_++;
        else
            remove_last();

        auto node = std::make_shared<Node>(key, value);
        map_[key] = node;
        insert_first(node);
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
