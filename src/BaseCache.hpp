#pragma once

template <typename KeyType, typename ValueType>
class BaseCache
{
  public:
    virtual bool      get(KeyType key, ValueType& result) = 0;
    virtual ValueType get(KeyType key)                    = 0;
    virtual void      put(KeyType key, ValueType value)   = 0;
};