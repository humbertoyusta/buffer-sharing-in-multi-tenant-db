#ifndef LIRS_H
#define LIRS_H

#include "types/page_access.h"
#include "types/tenant.h"
#include <list>
#include <queue>
#include <unordered_map>
#include <vector>

class LIRS {
public:
  struct Page {
    int page_id{0};
    int buffer_location{-1};
    bool is_lir{false};

    Page() {}

    Page(int page_id, int buffer_location, bool is_lir)
        : page_id(page_id), buffer_location(buffer_location), is_lir(is_lir) {}

    bool IsInCache() const { return buffer_location != -1; }
  };

  LIRS() {}

  LIRS(std::vector<Tenant> tenants, int total_buffer_size,
       double lirs_buffer_ratio_size);

  /**
   * @brief Access a page in the cache
   * @param page_access PageAccess object containing the tenant id and page id
   * @return std::pair<int, bool> where the first element is the buffer location
   * and the second is true if the page was found in the cache and false
   * otherwise
   */
  std::pair<int, bool> GetPage(PageAccess page_access);

  /**
   * @brief Evict a page from the cache of the given
   * tenant. The page in the bottom of the HIRS list will be evicted.
   * @return The evicted page
   * @note TurnLastLIRPageToHIR will be called if necessary
   */
  Page EvictPage(int tenant_id);

  /**
   * @brief Update the access history of the given page, if it is a LIR page it
   * will be moved to top of the LIRS list, otherwise, if it was in LIRS list
   * it will become a LIR page, otherwise it will be kept as a HIR page, in both
   * cases it will be moved to the top of respective list.
   * @param page_access PageAccess object containing the tenant id and page id
   * @note Page must be in the cache of the given tenant before calling this.
   * @note TurnLastLIRPageToHIR will be called if necessary
   */
  void UpdateAccessHistory(PageAccess page_access);

  /**
   * @brief Add the given page to the cache of the given tenant.
   * @param page_access PageAccess object containing the tenant id and page id
   * @param buffer_location Buffer location of the page
   * @note If the page is in the LIRS list, it will become a LIR page, and be
   * added to the top of the LIRS list.
   * @note Otherwise, it will be added to the top of the HIRS list.
   * @note TurnLastLIRPageToHIR will be called if necessary
   * @note Page must not be in the cache of the given tenant before calling
   * this.
   */
  void AddPage(PageAccess page_access, int buffer_location);

  /**
   * @brief Get the next available buffer location
   * @return The next available buffer location or -1 if there is no available
   */
  int GetAvailableLocation();

private:
  /**
   * @brief Prune the LIRS cache of the given tenant, removing the pages from
   * the bottom of the LIRS list, until the last page in the LIRS list is a LIR
   * page
   */
  void PruneLIRS(int tenant_id);

  /**
   * @brief Turn the last page in the LIRS list to a HIR page, calling PruneLIRS
   * if necessary
   */
  void TurnLastLIRPageToHIR(int tenant_id);

  /**
   * @brief Check if the last page in the LIRS should be turned to a HIR page
   * by comparing the size of the LIRS list with the size of the HIRS list
   */
  bool ShouldTurnLastLIRPageToHIR(int tenant_id);

  std::vector<Tenant> tenants_;
  int total_buffer_size_;
  std::queue<int> available_locations_;
  /**
   * Part of the buffer size that will be used for LIRS cache (1 -
   * lirs_buffer_ratio_size_) will be used for the HIRS cache
   */
  double lirs_buffer_ratio_size_;
  /**
   * Vector of integers, for LIRS sizes per tenant
   */
  std::vector<int> lirs_sizes_;
  /**
   * Vector of double linked lists, for LIRS list per tenant
   */
  std::vector<std::list<Page>> lirs_list_;
  /**
   * Vector of unordered maps, for LIRS list locations in list per tenant
   */
  std::vector<std::unordered_map<int, std::list<Page>::iterator>>
      lirs_locations_;
  /**
   * Vector of double linked lists, for HIRS list per tenant
   */
  std::vector<std::list<Page>> hirs_list_;
  /**
   * Vector of unordered maps, for HIRS list locations in list per tenant
   */
  std::vector<std::unordered_map<int, std::list<Page>::iterator>>
      hirs_locations_;
};

#endif // LIRS_H