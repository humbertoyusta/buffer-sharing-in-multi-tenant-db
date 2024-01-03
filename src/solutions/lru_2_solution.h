#ifndef LRU_2_SOLUTION_H
#define LRU_2_SOLUTION_H

#include "cache_eviction_algorithms/lru_2.h"
#include "single_lru_cache.h"
#include "solution.h"
#include "tenant_selection_policies/fault_ratio_cache_used_ratio_policy.h"

class Lru2Solution : public Solution {
public:
  Lru2Solution(double correlated_reference_period_length_multiplier)
      : correlated_reference_period_length_multiplier_(
            correlated_reference_period_length_multiplier) {}

  void Init(const std::vector<Tenant> &tenants, int total_buffer_size) override;

  std::pair<int, bool> AccessPage(PageAccess page_access) override;

private:
  double correlated_reference_period_length_multiplier_;
  void UpdateScore(int tenant_id);
  void AccessPageInJudge(PageAccess page_access);
  std::vector<SingleLruCache> judge_caches_;
  std::vector<int> faults_in_judge_;
  std::vector<int> faults_in_solution_;
  std::vector<int> cache_used_per_tenant_;
  std::vector<double> tenant_scores_;
  Lru2 lru2_cache_;
  FaultRatioCacheUsedRatioPolicy tenant_selection_policy_;
};

#endif // LRU_2_SOLUTION_H