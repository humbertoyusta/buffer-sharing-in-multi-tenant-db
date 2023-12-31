#ifndef SCORER_H
#define SCORER_H

#include "types/tenant.h"
#include <string>
#include <vector>

struct TestScore {
  double total_fault_score{0};
  std::vector<double> tenant_fault_scores;
  double total_hit_score{0};
  std::vector<double> tenant_hit_scores;
  std::vector<double> solution_hit_ratio_per_tenant;
  std::vector<double> judge_hit_ratio_per_tenant;
};

class Scorer {
public:
  Scorer(std::string solution_name);

  TestScore GetTestScore(std::vector<Tenant> tenants,
                         std::vector<int> judge_page_hits_per_tenant,
                         std::vector<int> judge_page_faults_per_tenant,
                         std::vector<int> solution_page_hits_per_tenant,
                         std::vector<int> solution_page_faults_per_tenant);

  void ReportScores(std::vector<TestScore> test_scores);

private:
  static double ratio(const int a, const int b);
  static double square(const double a);
  std::string solution_name_;
  std::vector<Tenant> tenants_;
};

#endif // SCORER_H