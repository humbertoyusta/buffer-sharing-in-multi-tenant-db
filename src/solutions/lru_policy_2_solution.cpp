#include "lru_policy_2_solution.h"
#include <assert.h>

void LruPolicy2Solution::Init(const std::vector<Tenant> &tenants,
                              int total_buffer_size) {
  lru_cache_ = Lru(tenants, total_buffer_size);
  tenant_selection_policy_ = HitRatioCacheUsedRatioPolicy();
  tenants_ = tenants;
  total_buffer_size_ = total_buffer_size;

  judge_caches_.clear();
  hits_in_judge_.clear();
  hits_in_solution_.clear();
  cache_used_per_tenant_.clear();

  for (const auto &tenant : tenants) {
    judge_caches_.emplace_back(tenant.base_buffer_size);
    hits_in_judge_.push_back(0);
    hits_in_solution_.push_back(0);
    cache_used_per_tenant_.push_back(0);
  }
}

void LruPolicy2Solution::AccessPageInJudge(PageAccess page_access) {
  auto judge_cache_result =
      judge_caches_[page_access.tenant_id - 1].AccessPage(page_access.page_id);
  if (judge_cache_result.second) {
    ++hits_in_judge_[page_access.tenant_id - 1];
  }
}

std::pair<int, bool> LruPolicy2Solution::AccessPage(PageAccess page_access) {

  AccessPageInJudge(page_access);

  auto found = lru_cache_.GetPage(page_access);

  if (found.second) {
    lru_cache_.SetPageAsMostRecentlyUsed(page_access);
    hits_in_solution_[page_access.tenant_id - 1]++;
    return {found.first, true};
  } else {

    int available_location = lru_cache_.GetAvailableLocation();
    if (available_location != -1 &&
        cache_used_per_tenant_[page_access.tenant_id - 1] <
            tenants_[page_access.tenant_id - 1].max_buffer_size) {
      lru_cache_.AddPage(page_access, available_location);
      ++cache_used_per_tenant_[page_access.tenant_id - 1];
      return {available_location, false};
    }

    std::vector<double> tenant_scores;
    for (const auto &tenant : tenants_) {
      if (hits_in_judge_[tenant.tenant_id - 1] == 0 ||
          tenant.base_buffer_size == 0) {
        tenant_scores.push_back(INT_MAX);
        continue;
      } else {
        tenant_scores.push_back(tenant_selection_policy_.TenantScore(
            hits_in_solution_[tenant.tenant_id - 1],
            hits_in_judge_[tenant.tenant_id - 1],
            cache_used_per_tenant_[tenant.tenant_id - 1],
            tenant.base_buffer_size, tenant.priority));
      }
    }

    for (int i = 0; i < tenants_.size(); ++i) {
      if (cache_used_per_tenant_[i] < tenants_[i].min_buffer_size ||
          (cache_used_per_tenant_[i] == tenants_[i].min_buffer_size &&
           i + 1 != page_access.tenant_id)) {
        tenant_scores[i] = INT_MAX;
      }
      if (cache_used_per_tenant_[page_access.tenant_id - 1] >=
              tenants_[page_access.tenant_id - 1].max_buffer_size &&
          i + 1 != page_access.tenant_id) {
        tenant_scores[i] = INT_MAX;
      }
    }

    assert(*std::min_element(tenant_scores.begin(), tenant_scores.end()) !=
           INT_MAX);

    int tenant_id_to_evict =
        std::min_element(tenant_scores.begin(), tenant_scores.end()) -
        tenant_scores.begin() + 1;

    std::pair<int, int> evicted_page =
        lru_cache_.EvictLeastRecentlyUsedPage(tenant_id_to_evict);

    --cache_used_per_tenant_[tenant_id_to_evict - 1];

    lru_cache_.AddPage(page_access, evicted_page.second);

    ++cache_used_per_tenant_[page_access.tenant_id - 1];

    assert(cache_used_per_tenant_[tenant_id_to_evict - 1] >=
           tenants_[tenant_id_to_evict - 1].min_buffer_size);

    return {evicted_page.second, false};
  }
}