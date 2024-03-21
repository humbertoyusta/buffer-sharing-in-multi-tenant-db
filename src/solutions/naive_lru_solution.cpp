#include "naive_lru_solution.h"
#include <assert.h>
#include <iostream>

void NaiveLruSolution::Init(const std::vector<Tenant> &tenants,
                            int total_buffer_size) {
  int sum_of_base_buffer_sizes = 0;
  for (auto tenant : tenants) {
    sum_of_base_buffer_sizes += tenant.base_buffer_size;
  }

  std::vector<int> tenant_buffer_sizes;
  int actual_total_buffer_size = 0;
  for (const auto &tenant : tenants) {
    tenant_buffer_sizes.push_back(
        int(round((double)tenant.base_buffer_size * (double)total_buffer_size /
                  (double)sum_of_base_buffer_sizes)));
    actual_total_buffer_size += tenant_buffer_sizes.back();
  }

  int round_robin_index = 0;
  while (actual_total_buffer_size < total_buffer_size) {
    if (tenant_buffer_sizes[round_robin_index] + 1 <=
        tenants[round_robin_index].max_buffer_size) {
      tenant_buffer_sizes[round_robin_index]++;
      actual_total_buffer_size++;
    }
    round_robin_index = (round_robin_index + 1) % tenants.size();
  }
  while (actual_total_buffer_size > total_buffer_size) {
    if (tenant_buffer_sizes[round_robin_index] - 1 >=
        tenants[round_robin_index].min_buffer_size) {
      tenant_buffer_sizes[round_robin_index]--;
      actual_total_buffer_size--;
    }
    round_robin_index = (round_robin_index + 1) % tenants.size();
  }

  for (int i = 0; i < tenants.size(); i++) {
    assert(tenant_buffer_sizes[i] >= tenants[i].min_buffer_size);
    assert(tenant_buffer_sizes[i] <= tenants[i].max_buffer_size);
  }

  cache_ranges_per_tenant_.clear();
  tenant_caches_.clear();

  int start = 1;
  for (int i = 0; i < tenants.size(); i++) {
    cache_ranges_per_tenant_.push_back(
        {start, start + tenant_buffer_sizes[i] - 1});
    start += tenant_buffer_sizes[i];
    tenant_caches_.emplace_back(tenant_buffer_sizes[i]);
  }
}

std::pair<int, bool> NaiveLruSolution::AccessPage(PageAccess page_access) {
  int tenant_id = page_access.tenant_id;
  int page_id = page_access.page_id;

  auto [buffer_location, hit] =
      tenant_caches_[tenant_id - 1].AccessPage(page_id);
  int actual_buffer_location =
      cache_ranges_per_tenant_[tenant_id - 1].first + buffer_location - 1;
  return {actual_buffer_location, hit};
}