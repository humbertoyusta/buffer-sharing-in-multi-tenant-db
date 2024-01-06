#ifndef _2Q_SOLUTION_H
#define _2Q_SOLUTION_H

#include "cache_eviction_algorithms/_2q.h"
#include "single_lru_cache.h"
#include "solution.h"
#include "tenant_selection_policies/fault_ratio_cache_used_ratio_policy.h"

class _2QSolution : public Solution {
public:
  _2QSolution(double a1_in_buffer_ratio_size, double a1_out_buffer_ratio_size)
      : a1_in_buffer_ratio_size_(a1_in_buffer_ratio_size),
        a1_out_buffer_ratio_size_(a1_out_buffer_ratio_size) {}

  void Init(const std::vector<Tenant> &tenants, int total_buffer_size) override;

  std::pair<int, bool> AccessPage(PageAccess page_access) override;

private:
  double a1_in_buffer_ratio_size_;
  double a1_out_buffer_ratio_size_;
  void UpdateScore(int tenant_id);
  void AccessPageInJudge(PageAccess page_access);
  std::vector<SingleLruCache> judge_caches_;
  std::vector<int> faults_in_judge_;
  std::vector<int> faults_in_solution_;
  std::vector<int> cache_used_per_tenant_;
  std::vector<double> tenant_scores_;
  _2Q _2q_cache_;
  FaultRatioCacheUsedRatioPolicy tenant_selection_policy_;
};

#endif // _2Q_SOLUTION_H