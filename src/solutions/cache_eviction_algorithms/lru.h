#ifndef LRU_H
#define LRU_H

#include "types/page_access.h"
#include "types/tenant.h"
#include <list>
#include <queue>
#include <vector>

class Lru {
public:
  Lru() {}

  Lru(std::vector<Tenant> tenants, int total_buffer_size);

  /**
   * @brief Access a page in the cache
   * @param page_access PageAccess object containing the tenant id and page id
   * @return std::pair<int, bool> where the first element is the page id and the
   * second is true if the page was found in the cache and false otherwise
   */
  std::pair<int, bool> GetPage(PageAccess page_access);

  /**
   * @brief Evict the least recently used page from the cache of the given
   * tenant.
   * @return std::pair<int, int> where the first element is the page id and the
   * second is the buffer location
   */
  std::pair<int, int> EvictLeastRecentlyUsedPage(int tenant_id);

  /**
   * @brief Set the given page as the most recently used page in the cache of
   * the given tenant.
   * @param page_access PageAccess object containing the tenant id and page id
   */
  void SetPageAsMostRecentlyUsed(PageAccess page_access);

  /**
   * @brief Add the given page to the cache of the given tenant, page_access
   * must not be in the cache of the given tenant before calling this
   * @param page_access PageAccess object containing the tenant id and page id
   * @param buffer_location Buffer location of the page
   */
  void AddPage(PageAccess page_access, int buffer_location);

  /**
   * @brief Get the next available buffer location
   * @return The next available buffer location or -1 if there is no available
   */
  int GetAvailableLocation();

private:
  std::vector<Tenant> tenants_;
  int total_buffer_size_;
  std::queue<int> available_locations_;
  std::vector<std::list<std::pair<int, int>>>
      pages_lists_; // Vector of double linked lists, for LRU cache per tenant,
                    // storing pairs of page id and buffer locations
  std::vector<std::unordered_map<int, std::list<std::pair<int, int>>::iterator>>
      pages_map_; // Vector of maps, for LRU cache per tenant, storing page id
                  // and iterator to the corresponding page in the double linked
                  // list
};

#endif // LRU_H