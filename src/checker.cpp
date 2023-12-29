#include "checker.h"
#include "single_lru_cache.h"
#include <assert.h>
#include <iostream>
#include <vector>

std::tuple<std::vector<int>, std::vector<int>, std::vector<int>,
           std::vector<int>>
Checker::CheckSolution(Solution *solution, const std::vector<Tenant> &tenants,
                       const std::vector<PageAccess> &page_accesses,
                       int total_buffer_size) {
  // Initialize the solution
  solution->Init(tenants, total_buffer_size);

  // Initialize the cache
  std::vector<SingleLruCache> judge_caches;
  for (const auto &tenant : tenants) {
    judge_caches.emplace_back(tenant.base_buffer_size);
  }

  std::vector<int> judge_page_hits_per_tenant(tenants.size(), 0);
  std::vector<int> solution_page_hits_per_tenant(tenants.size(), 0);

  std::vector<int> judge_page_faults_per_tenant(tenants.size(), 0);
  std::vector<int> solution_page_faults_per_tenant(tenants.size(), 0);

  std::vector<PageAccess> solution_buffer(total_buffer_size + 1);
  std::vector<int> solution_buffer_size_used_per_tenant(tenants.size(), 0);

  // Check each page access
  for (const auto &page_access : page_accesses) {
    // Get the tenant
    const auto &tenant = tenants[page_access.tenant_id - 1];

    // Get the judge cache
    auto &judge_cache = judge_caches[page_access.tenant_id - 1];

    // Access the page in the judge cache
    auto judge_result = judge_cache.AccessPage(page_access.page_id);

    // Access the page in the solution cache
    auto solution_result = solution->AccessPage(page_access);

    // Check the solution result
    if (solution_result.second) {
      // If it was a hit, check that the page is in the solution buffer in the
      // correct location
      assert(solution_buffer[solution_result.first] == page_access);
    } else {
      // If it was a fault, and the buffer is not full, check that
      // the tenant has not exceeded its max buffer size
      if (solution_buffer[solution_result.first].tenant_id == 0) {
        solution_buffer_size_used_per_tenant[page_access.tenant_id - 1]++;
        assert(
            solution_buffer_size_used_per_tenant[page_access.tenant_id - 1] <=
            tenant.max_buffer_size);
      }
      // If it was a fault, and it was evicted from the same tenant, check that
      // the tenant has at least its min buffer size
      else if (solution_buffer[solution_result.first].tenant_id ==
               tenant.tenant_id) {
        assert(
            solution_buffer_size_used_per_tenant[page_access.tenant_id - 1] >=
            tenant.min_buffer_size);
      } else {
        // If it was a fault, and it was evicted from a different tenant, check
        // that the tenant that was evicted has at least its min buffer size
        solution_buffer_size_used_per_tenant
            [solution_buffer[solution_result.first].tenant_id - 1]--;
        assert(solution_buffer_size_used_per_tenant
                   [solution_buffer[solution_result.first].tenant_id - 1] >=
               tenants[solution_buffer[solution_result.first].tenant_id - 1]
                   .min_buffer_size);

        // Check that the tenant that is accessing the page has not exceeded its
        // max buffer size
        solution_buffer_size_used_per_tenant[page_access.tenant_id - 1]++;
        assert(
            solution_buffer_size_used_per_tenant[page_access.tenant_id - 1] <=
            tenant.max_buffer_size);
      }
    }
    // Update the solution buffer
    solution_buffer[solution_result.first] = page_access;

    // Update the number of hits for the judge cache
    if (judge_result.second) {
      judge_page_hits_per_tenant[page_access.tenant_id - 1]++;
    } else {
      judge_page_faults_per_tenant[page_access.tenant_id - 1]++;
    }

    // Update the number of hits for the solution cache
    if (solution_result.second) {
      solution_page_hits_per_tenant[page_access.tenant_id - 1]++;
    } else {
      solution_page_faults_per_tenant[page_access.tenant_id - 1]++;
    }
  }

  for (const auto &tenant : tenants) {
    std::cout << "Tenant " << tenant.tenant_id << ":\n";
    std::cout << "  Judge hits: "
              << judge_page_hits_per_tenant[tenant.tenant_id - 1] << "\n";
    std::cout << "  Judge faults: "
              << judge_page_faults_per_tenant[tenant.tenant_id - 1] << "\n";
    std::cout
        << "  Judge hit rate: "
        << (double)judge_page_hits_per_tenant[tenant.tenant_id - 1] /
               (double)(judge_page_hits_per_tenant[tenant.tenant_id - 1] +
                        judge_page_faults_per_tenant[tenant.tenant_id - 1])
        << "\n";

    std::cout << "  Solution hits: "
              << solution_page_hits_per_tenant[tenant.tenant_id - 1] << "\n";
    std::cout << "  Solution faults: "
              << solution_page_faults_per_tenant[tenant.tenant_id - 1] << "\n";
    std::cout
        << "  Solution hit rate: "
        << (double)solution_page_hits_per_tenant[tenant.tenant_id - 1] /
               (double)(solution_page_hits_per_tenant[tenant.tenant_id - 1] +
                        solution_page_faults_per_tenant[tenant.tenant_id - 1])
        << "\n";
  }

  return std::make_tuple(
      judge_page_hits_per_tenant, judge_page_faults_per_tenant,
      solution_page_hits_per_tenant, solution_page_faults_per_tenant);
}