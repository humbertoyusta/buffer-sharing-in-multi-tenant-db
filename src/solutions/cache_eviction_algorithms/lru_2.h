#ifndef LRU_2_H
#define LRU_2_H

#include "types/page_access.h"
#include "types/tenant.h"
#include <list>
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

  Lru2(std::vector<Tenant> tenants, int total_buffer_size,
       double correlated_reference_period_length_multiplier,
       double retained_pages_period_length_multiplier);

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
  /**
   * @brief Move the last correlated page to the main cache
   * @param tenant_id The tenant id
   * @note The page must be in the correlated cache of the given tenant before
   * calling this
   */
  void MoveLastCorrelatedPageToMain(int tenant_id);
  /**
   * @brief Insert the given page (that was evicted from the main cache) to the
   * retained pages of the tenant, which preserves the page in the cache for a
   * certain period of time (retained period) so its information can be used to
   * predict future accesses
   * @param page_id The page id
   * @param tenant_id The tenant id
   * @param last_accessed_time The last accessed time of the page
   */
  void InsertToRetainedPages(int page_id, int tenant_id,
                             int last_accessed_time);
  std::vector<Tenant> tenants_;
  int total_buffer_size_;
  int current_time_{0};
  double correlated_reference_period_length_multiplier_;
  double retained_pages_period_length_multiplier_;
  std::vector<int> retained_period_length_;
  std::vector<int> correlated_reference_period_length_;
  std::queue<int> available_locations_;
  std::vector<std::set<Page>>
      pages_sets_; // Vector of sets of pages, for LRU-2 cache per tenant
  std::vector<std::list<Page>>
      correlated_pages_lists_; // Vector of lists of pages, for LRU-2 cache per
                               // tenant, storing pages in the order of their
                               // last access
  std::vector<std::unordered_map<int, std::list<Page>::iterator>>
      correlated_pages_maps_; // Vector of maps, for LRU-2 cache per tenant,
                              // storing page id and iterator to the
                              // corresponding page in the double linked list
  std::vector<std::unordered_map<int, std::set<Page>::iterator>>
      pages_maps_; // Vector of maps, for LRU cache per tenant, storing page id
  // and iterator to the corresponding page in the double
  // linked list
  std::vector<std::list<std::pair<int, int>>>
      retained_pages_lists_; // List of pairs of page id and last
                             // accessed time, for retained pages
  std::vector<std::unordered_map<int, std::list<std::pair<int, int>>::iterator>>
      retained_pages_maps_; // Map of page id and iterator to the
                            // corresponding page in the double linked
                            // list, for retained pages
};

#endif // LRU_2_H