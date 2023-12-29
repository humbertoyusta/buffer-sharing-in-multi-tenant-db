#ifndef FAULT_RATIO_CACHE_USED_RATIO_POLICY_H
#define FAULT_RATIO_CACHE_USED_RATIO_POLICY_H

class FaultRatioCacheUsedRatioPolicy {
public:
  static double TenantScore(int faults_in_solution, int faults_in_judge,
                            int cache_used, int base_buffer_size,
                            int priority) {
    return square(
               ratio(faults_in_solution - faults_in_judge, faults_in_judge)) *
           sign(faults_in_solution - faults_in_judge) * (double)priority /
           square(ratio(cache_used, base_buffer_size));
  }

private:
  static int sign(const int x) { return (x > 0) - (x < 0); }
  static double square(const double x) { return x * x; }
  static double ratio(const int x, const int y) {
    return static_cast<double>(x) / static_cast<double>(y);
  }
};

#endif // FAULT_RATIO_CACHE_USED_RATIO_POLICY_H