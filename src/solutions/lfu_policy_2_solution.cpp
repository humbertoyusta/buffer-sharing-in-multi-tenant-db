#include "lfu_policy_2_solution.h"
#include <assert.h>

void LfuPolicy2Solution::Init(const std::vector<Tenant> &tenants,
                              int total_buffer_size) {
  lfu_cache_ = Lfu(tenants, total_buffer_size);
  tenant_selection_policy_ = HitRatioCacheUsedRatioPolicy();
  tenants_ = tenants;
  total_buffer_size_ = total_buffer_size;

  judge_caches_.clear();
  hits_in_judge_.clear();
  hits_in_solution_.clear();
  cache_used_per_tenant_.clear();
  tenant_scores_.clear();

  for (const auto &tenant : tenants) {
    judge_caches_.emplace_back(tenant.base_buffer_size);
    hits_in_judge_.push_back(0);
    hits_in_solution_.push_back(0);
    cache_used_per_tenant_.push_back(0);
    tenant_scores_.push_back(0);
  }
}

void LfuPolicy2Solution::AccessPageInJudge(PageAccess page_access) {
  auto judge_cache_result =
      judge_caches_[page_access.tenant_id - 1].AccessPage(page_access.page_id);
  if (judge_cache_result.second) {
    ++hits_in_judge_[page_access.tenant_id - 1];
  }
}

std::pair<int, bool> LfuPolicy2Solution::AccessPage(PageAccess page_access) {

  AccessPageInJudge(page_access);

  auto found = lfu_cache_.GetPage(page_access);

  if (found.second) {
    lfu_cache_.IncreasePageFrequency(page_access);
    hits_in_solution_[page_access.tenant_id - 1]++;

    UpdateScore(page_access.tenant_id);
    return {found.first, true};
  } else {

    int available_location = lfu_cache_.GetAvailableLocation();
    if (available_location != -1 &&
        cache_used_per_tenant_[page_access.tenant_id - 1] <
            tenants_[page_access.tenant_id - 1].max_buffer_size) {
      lfu_cache_.AddPage(page_access, available_location);

      ++cache_used_per_tenant_[page_access.tenant_id - 1];

      UpdateScore(page_access.tenant_id);
      return {available_location, false};
    }

    int tenant_id_to_evict = -1;
    for (int tenant = 0; tenant < tenants_.size(); ++tenant) {
      if (cache_used_per_tenant_[tenant] < tenants_[tenant].min_buffer_size ||
          (cache_used_per_tenant_[tenant] == tenants_[tenant].min_buffer_size &&
           tenant + 1 != page_access.tenant_id)) {
        continue;
      }
      if (cache_used_per_tenant_[page_access.tenant_id - 1] >=
              tenants_[page_access.tenant_id - 1].max_buffer_size &&
          tenant + 1 != page_access.tenant_id) {
        continue;
      }
      if (tenant_id_to_evict == -1 ||
          tenant_scores_[tenant] < tenant_scores_[tenant_id_to_evict - 1]) {
        tenant_id_to_evict = tenant + 1;
      }
    }

    auto evicted_page =
        lfu_cache_.EvictLeastFrequentlyUsedPage(tenant_id_to_evict);

    --cache_used_per_tenant_[tenant_id_to_evict - 1];

    lfu_cache_.AddPage(page_access, evicted_page.buffer_location);

    ++cache_used_per_tenant_[page_access.tenant_id - 1];

    assert(cache_used_per_tenant_[tenant_id_to_evict - 1] >=
           tenants_[tenant_id_to_evict - 1].min_buffer_size);

    UpdateScore(page_access.tenant_id);
    UpdateScore(tenant_id_to_evict);
    return {evicted_page.buffer_location, false};
  }
}

void LfuPolicy2Solution::UpdateScore(int tenant_id) {
  if (hits_in_judge_[tenant_id - 1] == 0) {
    tenant_scores_[tenant_id - 1] = 0;
    return;
  }
  tenant_scores_[tenant_id - 1] = tenant_selection_policy_.TenantScore(
      hits_in_solution_[tenant_id - 1], hits_in_judge_[tenant_id - 1],
      cache_used_per_tenant_[tenant_id - 1],
      tenants_[tenant_id - 1].base_buffer_size,
      tenants_[tenant_id - 1].priority);
}