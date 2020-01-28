/*******************************************************************\
 
Module: LRU Cache
 
Author: Rafael Menezes, rafael.sa.menezes40@gmail.com
 
\*******************************************************************/

#ifndef LRU_CACHE
#define LRU_CACHE

#include <list>
#include <algorithm>
#include <unordered_map>

template <typename K, typename V>
class lru_cache
{
protected:
  typedef typename std::pair<K, V> KV;
  typedef typename std::list<KV>::iterator KV_iterator;
  // std::list is a double ended queue
  std::list<KV> cache;
  // a list has O(N) worst case, so it should be
  // faster to use a Hash for indexation O(1)
  std::unordered_map<K, KV_iterator> cache_index;
  size_t max_capacity = 0;

public:
  lru_cache(size_t length) : max_capacity(length)
  {
  }

  void insert(const K &key, const V &value)
  {
    cache.push_front(value);
    if(cache.size() > max_size())
    {
      cache.pop_back();
    }
  }

  V *get(const K &key)
  {
    return nullptr;
  }

  bool exists(const K &key)
  {
    return false;
  }

  size_t size()
  {
    return cache.size();
  }

  const size_t max_size()
  {
    return max_capacity;
  }
};
#endif
