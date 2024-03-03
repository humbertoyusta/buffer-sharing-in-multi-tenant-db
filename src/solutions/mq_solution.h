#ifndef MQ_SOLUTION_H
#define MQ_SOLUTION_H

#include "cache_eviction_algorithms/mq.h"
#include "single_lru_cache.h"
#include "solution.h"
#include "tenant_selection_policies/fault_ratio_cache_used_ratio_policy.h"

class MQSolution : public Solution {
public:
  MQSolution(int num_queues, double lifetime_length_multiplier,
             double retained_pages_period_length_multiplier)
      : num_queues_(num_queues),
        lifetime_length_multiplier_(lifetime_length_multiplier),
        retained_pages_period_length_multiplier_(
            retained_pages_period_length_multiplier) {}

  void Init(const std::vector<Tenant> &tenants, int total_buffer_size) override;

  std::pair<int, bool> AccessPage(PageAccess page_access) override;

private:
  int num_queues_;
  double lifetime_length_multiplier_;
  double retained_pages_period_length_multiplier_;
  void UpdateScore(int tenant_id);
  void AccessPageInJudge(PageAccess page_access);
  std::vector<SingleLruCache> judge_caches_;
  std::vector<int> faults_in_judge_;
  std::vector<int> faults_in_solution_;
  std::vector<int> cache_used_per_tenant_;
  std::vector<double> tenant_scores_;
  MQ mq_cache_;
  FaultRatioCacheUsedRatioPolicy tenant_selection_policy_;
};

#endif // MQ_SOLUTION_H