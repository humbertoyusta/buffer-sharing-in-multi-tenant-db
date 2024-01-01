#ifndef LRU_POLICY_2_SOLUTION_H
#define LRU_POLICY_2_SOLUTION_H

#include "cache_eviction_algorithms/lru.h"
#include "single_lru_cache.h"
#include "solution.h"
#include "tenant_selection_policies/hit_ratio_cache_used_ratio_policy.h"

class LruPolicy2Solution : public Solution {
public:
  LruPolicy2Solution() {}

  void Init(const std::vector<Tenant> &tenants, int total_buffer_size) override;

  std::pair<int, bool> AccessPage(PageAccess page_access) override;

private:
  void UpdateScore(int tenant_id);
  void AccessPageInJudge(PageAccess page_access);
  std::vector<SingleLruCache> judge_caches_;
  std::vector<int> hits_in_judge_;
  std::vector<int> hits_in_solution_;
  std::vector<int> cache_used_per_tenant_;
  std::vector<double> tenant_scores_;
  Lru lru_cache_;
  HitRatioCacheUsedRatioPolicy tenant_selection_policy_;
};

#endif // LRU_POLICY_2_SOLUTION_H