#ifndef SOLUTION_H
#define SOLUTION_H

#include "types/page_access.h"
#include "types/tenant.h"
#include <vector>

class Solution {
public:
  Solution() {}

  virtual ~Solution() {}

  virtual void init(const std::vector<Tenant> &tenants,
                    int total_buffer_size) = 0;

  /**
   * @brief Access a page in the cache
   * @return std::pair<int, int> where the first element is the tenant_id and
   * the second element is true if the page was found in the cache and false
   * otherwise
   */
  virtual std::pair<int, bool> AccessPage(PageAccess page_access) = 0;

protected:
  std::vector<Tenant> tenants_;
};

#endif // SOLUTION_H