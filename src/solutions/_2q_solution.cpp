#include "_2q_solution.h"
#include <assert.h>

void _2QSolution::Init(const std::vector<Tenant> &tenants,
                       int total_buffer_size) {
  _2q_cache_ = _2Q(tenants, total_buffer_size, a1_in_buffer_ratio_size_,
                   a1_out_buffer_ratio_size_);
  tenant_selection_policy_ = FaultRatioCacheUsedRatioPolicy();
  tenants_ = tenants;
  total_buffer_size_ = total_buffer_size;

  judge_caches_.clear();
  faults_in_judge_.clear();
  faults_in_solution_.clear();
  cache_used_per_tenant_.clear();
  tenant_scores_.clear();

  for (const auto &tenant : tenants) {
    judge_caches_.emplace_back(tenant.base_buffer_size);
    faults_in_judge_.push_back(0);
    faults_in_solution_.push_back(0);
    cache_used_per_tenant_.push_back(0);
    tenant_scores_.push_back(0);
  }
}

void _2QSolution::AccessPageInJudge(PageAccess page_access) {
  auto judge_cache_result =
      judge_caches_[page_access.tenant_id - 1].AccessPage(page_access.page_id);
  if (!judge_cache_result.second) {
    ++faults_in_judge_[page_access.tenant_id - 1];
  }
}

std::pair<int, bool> _2QSolution::AccessPage(PageAccess page_access) {

  AccessPageInJudge(page_access);

  auto found = _2q_cache_.GetPage(page_access);

  if (found.second) {
    _2q_cache_.SetPageAsMostRecentlyUsed(page_access);

    UpdateScore(page_access.tenant_id);
    return {found.first, true};
  } else {

    int available_location = _2q_cache_.GetAvailableLocation();
    if (available_location != -1 &&
        cache_used_per_tenant_[page_access.tenant_id - 1] <
            tenants_[page_access.tenant_id - 1].max_buffer_size) {
      _2q_cache_.AddPage(page_access, available_location);

      ++cache_used_per_tenant_[page_access.tenant_id - 1];
      ++faults_in_solution_[page_access.tenant_id - 1];

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

    std::pair<int, int> evicted_page = _2q_cache_.EvictPage(tenant_id_to_evict);

    --cache_used_per_tenant_[tenant_id_to_evict - 1];

    _2q_cache_.AddPage(page_access, evicted_page.second);

    ++cache_used_per_tenant_[page_access.tenant_id - 1];

    ++faults_in_solution_[page_access.tenant_id - 1];

    assert(cache_used_per_tenant_[tenant_id_to_evict - 1] >=
           tenants_[tenant_id_to_evict - 1].min_buffer_size);

    UpdateScore(page_access.tenant_id);
    UpdateScore(tenant_id_to_evict);
    return {evicted_page.second, false};
  }
}

void _2QSolution::UpdateScore(int tenant_id) {
  if (faults_in_judge_[tenant_id - 1] == 0) {
    tenant_scores_[tenant_id - 1] = 0;
    return;
  }
  tenant_scores_[tenant_id - 1] = tenant_selection_policy_.TenantScore(
      faults_in_solution_[tenant_id - 1], faults_in_judge_[tenant_id - 1],
      cache_used_per_tenant_[tenant_id - 1],
      tenants_[tenant_id - 1].base_buffer_size,
      tenants_[tenant_id - 1].priority);
}