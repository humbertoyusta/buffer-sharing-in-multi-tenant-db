#ifndef LFU_H
#define LFU_H

#include "types/page_access.h"
#include "types/tenant.h"
#include <list>
#include <map>
#include <queue>
#include <unordered_map>
#include <vector>

class Lfu {
public:
  struct Page {
    int page_id{0};
    int frequency{0};
    int buffer_location{0};

    Page(int page_id, int frequency, int buffer_location)
        : page_id(page_id), frequency(frequency),
          buffer_location(buffer_location) {}

    Page() {}
  };

  Lfu() {}

  Lfu(std::vector<Tenant> tenants, int total_buffer_size);

  /**
   * @brief Access a page in the cache
   * @param page_access PageAccess object containing the tenant id and page id
   * @return std::pair<int, bool> where the first element is the buffer location
   * and the second is true if the page was found in the cache and false
   * otherwise
   */
  std::pair<int, bool> GetPage(PageAccess page_access);

  /**
   * @brief Evict the least recently used page from the cache of the given
   * tenant.
   * @return The page that was evicted
   */
  Page EvictLeastFrequentlyUsedPage(int tenant_id);

  /**
   * @brief Increase the frequency of the given page in the cache of the given
   * @param page_access PageAccess object containing the tenant id and page id
   */
  void IncreasePageFrequency(PageAccess page_access);

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
  std::vector<std::map<int, std::list<Page>>> frequency_lists_;
  // Vector of maps, for LFU cache per tenant, storing page id, frequency and
  // buffer location
  std::vector<std::unordered_map<int, std::list<Page>::iterator>> pages_maps_;
  // Vector of maps, for LFU cache per tenant, storing page id and iterator to
  // the corresponding page in the frequency list
};

#endif // LFU_H