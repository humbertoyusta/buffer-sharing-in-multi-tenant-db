#ifndef CHECKER_H
#define CHECKER_H

#include "solutions/solution.h"

class Checker {
public:
  Checker() {}

  /**
   * @brief Check a solution against the judge (single-tenant-LRU), with given
   * tenants, page accesses and total buffer size
   * @return page_hits_in_judge, page_faults_in_judge, page_hits_in_solution,
   * page_faults_in_solution per tenant
   */
  static std::tuple<std::vector<int>, std::vector<int>, std::vector<int>,
                    std::vector<int>>
  CheckSolution(Solution *solution, const std::vector<Tenant> &tenants,
                const std::vector<PageAccess> &page_accesses,
                int total_buffer_size);
};

#endif // CHECKER_H