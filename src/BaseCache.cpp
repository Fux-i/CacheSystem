template <typename KeyType, typename ValueType>
class BaseCache
{
  public:
    virtual bool get(KeyType key, ValueType& result) = 0;
    virtual void put(KeyType key, ValueType value)   = 0;
};