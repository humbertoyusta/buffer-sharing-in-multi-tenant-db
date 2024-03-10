#ifndef BELADY_H
#define BELADY_H

#include "types/page_access.h"
#include "types/tenant.h"
#include <list>
#include <queue>
#include <set>
#include <unordered_map>
#include <vector>

class Belady {
public:
  struct Page {
    int page_id;
    int buffer_location;
    int next_access;

    Page() {}

    Page(int page_id, int buffer_location, int next_access)
        : page_id(page_id), buffer_location(buffer_location),
          next_access(next_access) {}

    bool operator<(const Page &other) const {
      if (next_access != other.next_access)
        return next_access > other.next_access;
      return page_id < other.page_id;
    }
  };

  Belady() {}

  /**
   * @brief Belady algorithm, note that this is the optimal algorithm but
   * requires the full sequence of page accesses to be known in advance, which
   * makes it impossible to use in a real scenario. This is only used for
   * comparison purposes.
   */
  Belady(std::vector<Tenant> tenants, int total_buffer_size,
         std::vector<PageAccess> page_accesses_sequence);

  /**
   * @brief Access a page in the cache
   * @param page_access PageAccess object containing the tenant id and page id
   * @return std::pair<int, bool> where the first element is the buffer location
   * and the second is true if the page was found in the cache and false
   * otherwise
   */
  std::pair<int, bool> GetPage(PageAccess page_access);

  /**
   * @brief Update the access history of the given page, updating next access
   * time of the page in the cache of the given tenant.
   * @param page_access PageAccess object containing the tenant id and page id
   * @note Page must be in the cache of the given tenant before calling this.
   */
  void UpdateAccessHistory(PageAccess page_access);

  /**
   * @brief Evict the page that will be accessed the farthest in the future from
   * the cache of the given tenant.
   * @return The page that was evicted
   * second is the buffer location
   */
  Page EvictPage(int tenant_id);

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
  int current_time_;

  /**
   * @brief Update the current time and the next accesses of the pages in the
   * cache, increasing the current time by 1 and updating the next accesses of
   * the page that was accessed
   */
  void UpdateCurrentTimeAndNextAccesses(PageAccess page_access);

  /**
   * @brief Initialize the next accesses of each page in the cache, by index in
   * the sequence
   */
  void InitNextAccessesByIndex();

  /**
   * @brief Initialize the next accesses of each page in the cache, at the
   * beginning
   */
  void InitNextAccessesByPage();

  std::vector<Tenant> tenants_;
  int total_buffer_size_;
  std::queue<int> available_locations_;

  std::vector<PageAccess> page_accesses_sequence_;

  // Vector of sets, for pages per tenant in the cache
  std::vector<std::set<Page>> pages_sets_;

  // The next access of each element in the sequence, by index in the sequence
  std::vector<int> next_accesses_by_index_;

  // The next access of each page, by page id
  std::vector<std::unordered_map<int, int>> next_accesses_by_page_;

  // Vector of maps, of iterators to the corresponding page in the set
  std::vector<std::unordered_map<int, std::set<Page>::iterator>>
      pages_locations_;
};

#endif // BELADY_H