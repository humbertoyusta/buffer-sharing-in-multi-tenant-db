#include "lfu.h"
#include <assert.h>

Lfu::Lfu(std::vector<Tenant> tenants, int total_buffer_size)
    : tenants_(tenants), total_buffer_size_(total_buffer_size) {
  for (int i = 1; i <= total_buffer_size; i++) {
    available_locations_.push(i);
  }

  frequency_lists_.resize(tenants.size());
  pages_maps_.resize(tenants.size());
}

std::pair<int, bool> Lfu::GetPage(PageAccess page_access) {
  auto found = pages_maps_[page_access.tenant_id - 1].find(page_access.page_id);

  if (found != pages_maps_[page_access.tenant_id - 1].end()) {
    return {found->second->buffer_location, true};
  } else {
    return {0, false};
  }
}

Lfu::Page Lfu::EvictLeastFrequentlyUsedPage(int tenant_id) {
  assert(!frequency_lists_[tenant_id - 1].empty());

  auto min_frequency_list_iterator = frequency_lists_[tenant_id - 1].begin();

  auto page_to_evict = min_frequency_list_iterator->second.back();

  pages_maps_[tenant_id - 1].erase(page_to_evict.page_id);

  min_frequency_list_iterator->second.pop_back();

  if (min_frequency_list_iterator->second.empty()) {
    frequency_lists_[tenant_id - 1].erase(min_frequency_list_iterator);
  }

  return page_to_evict;
}

void Lfu::IncreasePageFrequency(PageAccess page_access) {
  auto found = pages_maps_[page_access.tenant_id - 1].find(page_access.page_id);
  assert(found != pages_maps_[page_access.tenant_id - 1].end());

  int old_frequency = found->second->frequency;
  int new_frequency = old_frequency + 1;
  int buffer_location = found->second->buffer_location;

  frequency_lists_[page_access.tenant_id - 1][old_frequency].erase(
      found->second);

  if (frequency_lists_[page_access.tenant_id - 1][old_frequency].empty()) {
    frequency_lists_[page_access.tenant_id - 1].erase(old_frequency);
  }

  frequency_lists_[page_access.tenant_id - 1][new_frequency].push_front(
      {page_access.page_id, new_frequency, buffer_location});

  pages_maps_[page_access.tenant_id - 1][page_access.page_id] =
      frequency_lists_[page_access.tenant_id - 1][new_frequency].begin();
}

void Lfu::AddPage(PageAccess page_access, int buffer_location) {
  assert(pages_maps_[page_access.tenant_id - 1].find(page_access.page_id) ==
         pages_maps_[page_access.tenant_id - 1].end());

  frequency_lists_[page_access.tenant_id - 1][1].push_front(
      {page_access.page_id, 1, buffer_location});

  pages_maps_[page_access.tenant_id - 1][page_access.page_id] =
      frequency_lists_[page_access.tenant_id - 1][1].begin();
}

int Lfu::GetAvailableLocation() {
  if (available_locations_.empty()) {
    return -1;
  }
  int location = available_locations_.front();
  available_locations_.pop();
  return location;
}
