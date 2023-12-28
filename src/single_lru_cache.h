#ifndef SINGLE_LRU_CACHE_H
#define SINGLE_LRU_CACHE_H

#include <list>
#include <queue>
#include <unordered_map>

class SingleLruCache {
public:
  SingleLruCache(int buffer_size);
  std::pair<int, bool> AccessPage(int page_id);

private:
  int buffer_size_;
  std::list<std::pair<int, int>>
      pages_; // List to store pairs of page_id and buffer locations
  std::unordered_map<int, std::list<std::pair<int, int>>::iterator>
      page_map_; // Map to store page_id and its position in list
  std::queue<int> available_locations_; // Queue of available buffer locations
};

#endif // SINGLE_LRU_CACHE_H
