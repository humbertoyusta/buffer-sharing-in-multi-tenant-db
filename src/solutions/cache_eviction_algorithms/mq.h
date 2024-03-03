#ifndef MQ_H
#define MQ_H

#include "types/page_access.h"
#include "types/tenant.h"
#include <list>
#include <queue>
#include <set>
#include <unordered_map>
#include <vector>

class MQ {
public:
  struct Page {
    int page_id{0};
    int buffer_location{0};
    int reference_count{0};
    int expiration_time{0};

    Page(int page_id, int buffer_location, int reference_count,
         int expiration_time)
        : page_id(page_id), buffer_location(buffer_location),
          reference_count(reference_count), expiration_time(expiration_time) {}

    Page() {}
  };

  struct RetainedPage {
    int tenant_id{0};
    int page_id{0};
    int reference_count{0};

    RetainedPage(int tenant_id, int page_id, int reference_count)
        : tenant_id(tenant_id), page_id(page_id),
          reference_count(reference_count) {}

    RetainedPage() {}
  };

  MQ() {}

  MQ(std::vector<Tenant> tenants, int total_buffer_size, int num_queues,
     double lifetime_length_multiplier,
     double retained_pages_period_length_multiplier);

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

  /**
   * @brief Adjust the queues based on the current time, moving pages to
   * previous queues if their expiration time has passed, also increasing the
   * current time
   */
  void AdjustQueues();

private:
  /**
   * @brief Add the given page to the retained pages of the given tenant
   */
  void AddPageToRetainedPages(Page page, int tenant_id);

  /**
   * @brief Get the queue number based on the reference count (logarithmic
   * scale)
   */
  int GetQueueNumberBasedOnReferenceCount(int reference_count);

  int current_time_{0};
  int num_queues_; // number of queues (A1, A2, A3, ..., Anum_queues) per tenant
  /** Lifetime of a page, time without being referenced before being moved to
   * the previous queue(A1<-A2<-A3<-...<-Anum_queues), ratio with respect to
   * base buffer size of tenant */
  double lifetime_length_multiplier_;
  std::vector<int> lifetimes_; // lifetime of each tenant
  /**
   * Period of time a page is retained in the cache after being evicted, ratio
   * with respect to base buffer size of tenant
   */
  double retained_pages_period_length_multiplier_;
  std::vector<int> retained_periods_; // retained period of each tenant
  std::vector<Tenant> tenants_;
  int total_buffer_size_;
  std::queue<int> available_locations_;
  // For each tenant, vector of queues (A1, A2, A3, ..., Anum_queues)
  std::vector<std::vector<std::list<Page>>> cache_queues_;
  // tenant_id, page_id -> queue_index, iterator
  std::vector<
      std::unordered_map<int, std::pair<int, std::list<Page>::iterator>>>
      cache_locations_;
  // For each tenant, list of retained pages
  std::vector<std::list<RetainedPage>> retained_pages_;
  // For each tenant, where the retained pages are located in the list
  std::vector<std::unordered_map<int, std::list<RetainedPage>::iterator>>
      retained_pages_locations_;
};

#endif // MQ_H