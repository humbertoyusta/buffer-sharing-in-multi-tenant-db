#include "lru_2.h"
#include <assert.h>

Lru2::Lru2(std::vector<Tenant> tenants, int total_buffer_size)
    : tenants_(tenants), total_buffer_size_(total_buffer_size) {
  for (int i = 1; i <= total_buffer_size; ++i) {
    available_locations_.push(i);
  }
  pages_sets_.resize(tenants.size());
  pages_maps_.resize(tenants.size());
}

std::pair<int, bool> Lru2::GetPage(PageAccess page_access) {
  auto found = pages_maps_[page_access.tenant_id - 1].find(page_access.page_id);

  if (found != pages_maps_[page_access.tenant_id - 1].end()) {
    return {found->second->buffer_location, true};
  } else {
    return {0, false};
  }
}

Lru2::Page Lru2::EvictPage(int tenant_id) {
  assert(!pages_sets_[tenant_id - 1].empty());
  auto page_to_evict_iterator = pages_sets_[tenant_id - 1].begin();
  auto page_to_evict = *page_to_evict_iterator;
  pages_maps_[tenant_id - 1].erase(page_to_evict.page_id);
  pages_sets_[tenant_id - 1].erase(page_to_evict_iterator);
  return page_to_evict;
}

void Lru2::UpdateAccessHistory(PageAccess page_access) {
  ++current_time_;
  auto found = pages_maps_[page_access.tenant_id - 1].find(page_access.page_id);
  auto page = *found->second;
  pages_sets_[page_access.tenant_id - 1].erase(found->second);

  page.second_last_accessed = page.last_accessed;
  page.last_accessed = current_time_;

  auto insert_result = pages_sets_[page_access.tenant_id - 1].insert(page);
  assert(insert_result.second);

  pages_maps_[page_access.tenant_id - 1][page_access.page_id] =
      insert_result.first;
}

void Lru2::AddPage(PageAccess page_access, int buffer_location) {
  ++current_time_;
  Page page = Page(page_access.page_id, buffer_location, current_time_, 0);
  auto insert_result = pages_sets_[page_access.tenant_id - 1].insert(page);
  assert(insert_result.second);
  pages_maps_[page_access.tenant_id - 1][page_access.page_id] =
      insert_result.first;
}

int Lru2::GetAvailableLocation() {
  if (available_locations_.empty()) {
    return -1;
  }
  int available_location = available_locations_.front();
  available_locations_.pop();
  return available_location;
}