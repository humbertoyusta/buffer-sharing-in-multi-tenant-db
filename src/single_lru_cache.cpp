#include "single_lru_cache.h"

SingleLruCache::SingleLruCache(int buffer_size) : buffer_size_(buffer_size) {
  for (int i = 1; i <= buffer_size; ++i) {
    available_locations_.push(i);
  }
}

std::pair<int, bool> SingleLruCache::AccessPage(int page_id) {
  auto found = page_map_.find(page_id);

  // If not found, it's a miss
  if (found == page_map_.end()) {
    int buffer_location;
    if (pages_.size() == buffer_size_) {
      // Remove the least recently used page
      buffer_location = pages_.back().second;
      page_map_.erase(pages_.back().first);
      pages_.pop_back();
    } else {
      // Get the next available buffer location
      buffer_location = available_locations_.front();
      available_locations_.pop();
    }

    // Add the new page to the front of the list
    pages_.push_front({page_id, buffer_location});
    page_map_[page_id] = pages_.begin();
    return {buffer_location,
            false}; // Return buffer location and 'false' for miss
  }

  // If found, it's a hit
  // Move the page to the front of the list
  int buffer_location = found->second->second;
  pages_.erase(found->second);
  pages_.push_front({page_id, buffer_location});
  page_map_[page_id] = pages_.begin();
  return {buffer_location, true}; // Return buffer location and 'true' for hit
}
