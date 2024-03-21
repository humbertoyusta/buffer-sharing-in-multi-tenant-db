#ifndef NAIVE_LRU_SOLUTION_H
#define NAIVE_LRU_SOLUTION_H

#include "single_lru_cache.h"
#include "solution.h"

class NaiveLruSolution : public Solution {
public:
  NaiveLruSolution() {}

  void Init(const std::vector<Tenant> &tenants, int total_buffer_size) override;

  std::pair<int, bool> AccessPage(PageAccess page_access) override;

private:
  std::vector<std::pair<int, int>> cache_ranges_per_tenant_;
  std::vector<SingleLruCache> tenant_caches_;
};

#endif // LRU_POLICY_1_SOLUTION_H