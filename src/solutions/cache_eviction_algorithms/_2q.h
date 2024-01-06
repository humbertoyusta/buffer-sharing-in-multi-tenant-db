#ifndef _2Q_H
#define _2Q_H

#include "types/page_access.h"
#include "types/tenant.h"
#include <list>
#include <queue>
#include <unordered_map>
#include <vector>

class _2Q {
public:
  _2Q() {}

  _2Q(std::vector<Tenant> tenants, int total_buffer_size,
      double a1_in_buffer_ratio_size, double a1_out_buffer_ratio_size);

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
   * @return std::pair<int, int> where the first element is the page id and the
   * second is the buffer location
   */
  std::pair<int, int> EvictPage(int tenant_id);

  /**
   * @brief Set the given page as the most recently used page in the cache of
   * the given tenant, if it was in A1 in queue, move it to Am queue, if it was
   * in Am queue, move it to the front of the queue
   * @param page_access PageAccess object containing the tenant id and page id
   * @note Page must be in the cache of the given tenant before calling this,
   * that is it must be in A1 in queue or Am queue
   */
  void SetPageAsMostRecentlyUsed(PageAccess page_access);

  /**
   * @brief Add the given page to the cache of the given tenant, page_access
   * must not be in the cache of the given tenant before calling this
   * @param page_access PageAccess object containing the tenant id and page id
   * @param buffer_location Buffer location of the page
   * @note If page is in A1 out queue, it will be moved to Am queue, otherwise
   * to A1 in queue
   */
  void AddPage(PageAccess page_access, int buffer_location);

  /**
   * @brief Get the next available buffer location
   * @return The next available buffer location or -1 if there is no available
   */
  int GetAvailableLocation();

private:
  /**
   * @brief Add the given page to the A1 out queue of the given tenant, if
   * necessary, evict the last page from the A1 out queue
   * @note page_access must not be in the A1 in, A1 out or Am queues of the
   * given tenant before calling this
   */
  void AddPageToA1OutQueue(int tenant_id, int page_id);
  std::vector<Tenant> tenants_;
  int total_buffer_size_;
  std::queue<int> available_locations_;
  double a1_in_buffer_ratio_size_;  // Size of A1 in queue as a ratio of the
                                    // buffer size used per tenant
  double a1_out_buffer_ratio_size_; // Size of A1 out queue as a ratio of the
                                    // base buffer size of the tenant
  std::vector<std::list<std::pair<int, int>>>
      a1_in_queue_; // Vector of double linked lists, for A1 in queue per
                    // tenant, stores page id and buffer location
  std::vector<std::unordered_map<int, std::list<std::pair<int, int>>::iterator>>
      a1_in_maps_; // Vector of maps, for A1 in queue per tenant, storing page
                   // id and iterator to the corresponding page in the double
                   // linked list
  std::vector<std::list<int>>
      a1_out_queue_; // Vector of double linked lists,
                     // for A1 out queue per tenant, it does not store the
                     // buffer location since page is not in the buffer
  std::vector<std::unordered_map<int, std::list<int>::iterator>>
      a1_out_maps_; // Vector of maps, for A1 out queue per tenant, storing page
                    // id and iterator to the corresponding page in the double
                    // linked list
  std::vector<std::list<std::pair<int, int>>>
      am_queue_; // Vector of double linked lists, for Am queue per tenant,
                 // stores page id and buffer location
  std::vector<std::unordered_map<int, std::list<std::pair<int, int>>::iterator>>
      am_maps_; // Vector of maps, for Am queue per tenant, storing page id and
                // iterator to the corresponding page in the double linked list
};

#endif // _2Q_H