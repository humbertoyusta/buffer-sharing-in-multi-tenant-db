#include "checker.h"
#include "single_lru_cache.h"

#include <iostream>

void Checker::CheckSolution(Solution *solution,
                            const std::vector<Tenant> &tenants,
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
}