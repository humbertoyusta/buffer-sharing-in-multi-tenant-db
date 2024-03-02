#ifndef LRFU_2_SOLUTION_H
#define LRFU_2_SOLUTION_H

#include "cache_eviction_algorithms/lrfu.h"
#include "single_lru_cache.h"
#include "solution.h"
#include "tenant_selection_policies/fault_ratio_cache_used_ratio_policy.h"

class LRFUSolution : public Solution {
public:
  LRFUSolution(double lambda) : lambda_(lambda) {}

  void Init(const std::vector<Tenant> &tenants, int total_buffer_size) override;

  std::pair<int, bool> AccessPage(PageAccess page_access) override;

private:
  double lambda_;
  void UpdateScore(int tenant_id);
  void AccessPageInJudge(PageAccess page_access);
  std::vector<SingleLruCache> judge_caches_;
  std::vector<int> faults_in_judge_;
  std::vector<int> faults_in_solution_;
  std::vector<int> cache_used_per_tenant_;
  std::vector<double> tenant_scores_;
  LRFU lrfu_cache_;
  FaultRatioCacheUsedRatioPolicy tenant_selection_policy_;
};

#endif // LRFU_2_SOLUTION_H