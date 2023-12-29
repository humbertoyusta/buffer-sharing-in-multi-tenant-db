#include "lru.h"
#include <assert.h>

Lru::Lru(std::vector<Tenant> tenants, int total_buffer_size)
    : tenants_(tenants), total_buffer_size_(total_buffer_size) {
  for (int i = 1; i <= total_buffer_size; ++i) {
    available_locations_.push(i);
  }
  pages_lists_.resize(tenants.size());
  pages_maps_.resize(tenants.size());
}

std::pair<int, bool> Lru::GetPage(PageAccess page_access) {
  auto found = pages_maps_[page_access.tenant_id - 1].find(page_access.page_id);

  if (found != pages_maps_[page_access.tenant_id - 1].end()) {
    int buffer_location = found->second->second;
    return {buffer_location, true};
  } else {
    return {0, false};
  }
}

std::pair<int, int> Lru::EvictLeastRecentlyUsedPage(int tenant_id) {
  assert(!pages_lists_[tenant_id - 1].empty());
  auto page_to_evict = pages_lists_[tenant_id - 1].back();
  pages_maps_[tenant_id - 1].erase(page_to_evict.first);
  pages_lists_[tenant_id - 1].pop_back();
  return page_to_evict;
}

void Lru::SetPageAsMostRecentlyUsed(PageAccess page_access) {
  auto found = pages_maps_[page_access.tenant_id - 1].find(page_access.page_id);
  int buffer_location = found->second->second;
  assert(found != pages_maps_[page_access.tenant_id - 1].end());

  pages_lists_[page_access.tenant_id - 1].erase(found->second);
  pages_lists_[page_access.tenant_id - 1].push_front(
      {page_access.page_id, buffer_location});

  pages_maps_[page_access.tenant_id - 1][page_access.page_id] =
      pages_lists_[page_access.tenant_id - 1].begin();
}

void Lru::AddPage(PageAccess page_access, int buffer_location) {
  assert(pages_maps_[page_access.tenant_id - 1].find(page_access.page_id) ==
         pages_maps_[page_access.tenant_id - 1].end());

  pages_lists_[page_access.tenant_id - 1].push_front(
      {page_access.page_id, buffer_location});

  pages_maps_[page_access.tenant_id - 1][page_access.page_id] =
      pages_lists_[page_access.tenant_id - 1].begin();
}

int Lru::GetAvailableLocation() {
  if (available_locations_.empty()) {
    return -1;
  }
  int location = available_locations_.front();
  available_locations_.pop();
  return location;
}