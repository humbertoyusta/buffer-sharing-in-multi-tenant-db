#ifndef LRU_POLICY_1_SOLUTION_H
#define LRU_POLICY_1_SOLUTION_H

#include "cache_eviction_algorithms/lru.h"
#include "single_lru_cache.h"
#include "solution.h"
#include "tenant_selection_policies/fault_ratio_cache_used_ratio_policy.h"

class LruPolicy1Solution : public Solution {
public:
  LruPolicy1Solution() {}

  void Init(const std::vector<Tenant> &tenants, int total_buffer_size) override;

  std::pair<int, bool> AccessPage(PageAccess page_access) override;

private:
  void AccessPageInJudge(PageAccess page_access);
  std::vector<SingleLruCache> judge_caches_;
  std::vector<int> faults_in_judge_;
  std::vector<int> faults_in_solution_;
  std::vector<int> cache_used_per_tenant_;
  Lru lru_cache_;
  FaultRatioCacheUsedRatioPolicy tenant_selection_policy_;
};

#endif // LRU_POLICY_1_SOLUTION_H