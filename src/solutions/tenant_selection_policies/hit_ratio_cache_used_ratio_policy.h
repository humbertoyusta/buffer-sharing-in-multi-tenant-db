#ifndef HIT_RATIO_CACHE_USED_RATIO_POLICY_H
#define HIT_RATIO_CACHE_USED_RATIO_POLICY_H

class HitRatioCacheUsedRatioPolicy {
public:
  static double TenantScore(int hits_in_solution, int hits_in_judge,
                            int cache_used, int base_buffer_size,
                            int priority) {
    return square(ratio(hits_in_judge - hits_in_solution, hits_in_judge)) *
           sign(hits_in_judge - hits_in_solution) * (double)priority /
           square(ratio(cache_used, base_buffer_size));
  }

private:
  static int sign(const int x) { return (x > 0) - (x < 0); }
  static double square(const double x) { return x * x; }
  static double ratio(const int x, const int y) {
    return static_cast<double>(x) / static_cast<double>(y);
  }
};

#endif // HIT_RATIO_CACHE_USED_RATIO_POLICY_H