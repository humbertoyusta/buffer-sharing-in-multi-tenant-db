#ifndef LRU_2_H
#define LRU_2_H

#include "types/page_access.h"
#include "types/tenant.h"
#include <queue>
#include <set>
#include <unordered_map>
#include <vector>

class Lru2 {
public:
  struct Page {
    int page_id{0};
    int buffer_location{0};
    int last_accessed{0};
    int second_last_accessed{0};

    Page(int t_page_id, int t_buffer_location, int t_last_accessed,
         int t_second_last_accessed)
        : page_id(t_page_id), buffer_location(t_buffer_location),
          last_accessed(t_last_accessed),
          second_last_accessed(t_second_last_accessed) {}

    Page() {}

    // Compare the second last accessed time first, then the last accessed time
    // to break ties, as used in the LRU-2 algorithm for the 2-backward distance
    bool operator<(const Page &other) const {
      if (second_last_accessed == other.second_last_accessed) {
        return last_accessed < other.last_accessed;
      }
      return second_last_accessed < other.second_last_accessed;
    }
  };

  Lru2() {}

  Lru2(std::vector<Tenant> tenants, int total_buffer_size);

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
   * @return The page that was evicted
   */
  Page EvictPage(int tenant_id);

  /**
   * @brief Update the access history of the given page
   */
  void UpdateAccessHistory(PageAccess page_access);

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
  int current_time_{0};
  std::queue<int> available_locations_;
  std::vector<std::set<Page>>
      pages_sets_; // Vector of sets of pages, for LRU-2 cache per tenant
  std::vector<std::unordered_map<int, std::set<Page>::iterator>>
      pages_maps_; // Vector of maps, for LRU cache per tenant, storing page id
                   // and iterator to the corresponding page in the double
                   // linked list
};

#endif // LRU_2_H