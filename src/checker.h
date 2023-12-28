#ifndef CHECKER_H
#define CHECKER_H

#include "solutions/solution.h"

class Checker {
public:
  Checker() {}

  static void CheckSolution(/*Solution *solution,*/
                            const std::vector<Tenant> &tenants,
                            const std::vector<PageAccess> &page_accesses,
                            int total_buffer_size);
};

#endif // CHECKER_H