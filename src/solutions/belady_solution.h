#ifndef BELADY_SOLUTION_H
#define BELADY_SOLUTION_H

#include "cache_eviction_algorithms/belady.h"
#include "single_lru_cache.h"
#include "solution.h"
#include "tenant_selection_policies/fault_ratio_cache_used_ratio_policy.h"

/**
 * @brief BeladySolution, note that this is the optimal algorithm but requires
 * the full sequence of page accesses to be known in advance, which makes it
 * impossible to use in a real scenario. This is only used for comparison
 * purposes.
 */
class BeladySolution : public Solution {
public:
  BeladySolution(std::vector<PageAccess> page_accesses_sequence)
      : page_accesses_sequence_(page_accesses_sequence){};

  void Init(const std::vector<Tenant> &tenants, int total_buffer_size) override;

  std::pair<int, bool> AccessPage(PageAccess page_access) override;

private:
  std::vector<PageAccess> page_accesses_sequence_;
  double lirs_buffer_ratio_size_;
  void UpdateScore(int tenant_id);
  void AccessPageInJudge(PageAccess page_access);
  std::vector<SingleLruCache> judge_caches_;
  std::vector<int> faults_in_judge_;
  std::vector<int> faults_in_solution_;
  std::vector<int> cache_used_per_tenant_;
  std::vector<double> tenant_scores_;
  Belady belady_cache_;
  FaultRatioCacheUsedRatioPolicy tenant_selection_policy_;
};

#endif // BELADY_SOLUTION_H