#ifndef LRFU_H
#define LRFU_H

#include "types/page_access.h"
#include "types/tenant.h"
#include <list>
#include <queue>
#include <set>
#include <unordered_map>
#include <vector>

class LRFU {
public:
  class Page {
  public:
    int page_id{0};
    int buffer_location{0};
    int last_access_time{0};
    double last_CRF_value{0}; // Last Computed Reference Frequency value

    Page(int page_id, int buffer_location, int last_access_time,
         double last_CRF_value)
        : page_id(page_id), buffer_location(buffer_location),
          last_access_time(last_access_time), last_CRF_value(last_CRF_value) {}

    Page() {}

    // Get the Computed Reference Frequency value of the page at the given time
    double getCRFValue(int current_time) const;

    // Compare two pages based on their CRF values
    bool operator<(const Page &other) const;
  };

  LRFU() {}

  LRFU(std::vector<Tenant> tenants, int total_buffer_size, double lambda);

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
  Page EvictPage(int tenant_id);

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
  static int current_time_;
  static double lambda_; // Lambda LRFU parameter, determines the rate of
                         // exponential decay of the page reference frequency
  static double two_to_the_power_minus_lambda_; // 2^-lambda
  std::vector<Tenant> tenants_;
  int total_buffer_size_;
  std::queue<int> available_locations_;
  std::vector<std::set<Page>> pages_sets_;
  std::vector<std::unordered_map<int, std::set<Page>::iterator>> pages_maps_;
};

#endif // LRFU_H